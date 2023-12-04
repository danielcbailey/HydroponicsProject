package asynchronousactivities

import (
	"strconv"
	"time"

	"github.com/danielcbailey/HydroponicsProject/server/common"
)

func performAnalysis(readings common.SensorReadings) {
	// analysis includes:
	// - checking for water leaks
	// - checking for light degredation
	// - checking for excess (or too little) CO2 levels
	// - checking for excess (or too little) air temperature
	// - checking for excess (or too little) air humidity
	// - checking for excess (or too little) pH
	// - checking for excess (or too little) EC
	// - checking for low water levels
	checkForWaterLeaks(readings)
	checkForLightDegredation(readings)
	checkForCO2Excess(readings)
	checkForAirTempExcess(readings)
	checkForAirHumidityExcess(readings)
	checkForPHExcess(readings)
	checkForECExcess(readings)
}

var timeLastSentLeakNotification time.Time

func checkForWaterLeaks(readings common.SensorReadings) {
	if timeLastSentLeakNotification.Add(24 * time.Hour).After(time.Now()) {
		return
	}
	// if the pump was on, but turned off a minute ago, we can check for leaks during the pump's operation.
	// the config's leak threshold is in mL/min, so we need to figure out how much water the pump moved while
	// it was on, and the change in water level in the tank.

	flowMeasurements, err := common.GetPastSensorMeasurements(common.SensorMeasurementNames.CurrentLiquidFlow, 60*time.Minute)
	if err != nil {
		common.LogF(common.SeverityWarning, "failed to check for water leaks because the database failed: %v", err)
		return
	}

	if len(flowMeasurements) < 2 {
		common.LogF(common.SeverityInfo, "not enough measurements to check for water leaks")
		return
	}

	// the pump must have been on during the last minute, but just turned off
	if flowMeasurements[0].Value == 0 || readings.CurrentLiquidFlow > 0 {
		return
	}

	levelMeasurements, err := common.GetPastSensorMeasurements(common.SensorMeasurementNames.WaterLevel, 60*time.Minute)
	if err != nil {
		common.LogF(common.SeverityWarning, "failed to check for water leaks because the database failed: %v", err)
		return
	}

	if len(levelMeasurements) != len(flowMeasurements) {
		common.LogF(common.SeverityWarning, "failed to check for water leaks because the number of measurements for water level and flow are different")
		return
	}

	// the first measurement is the most recent
	pumpOnIndex := len(flowMeasurements) - 1
	for i := 0; i < len(flowMeasurements); i++ {
		if flowMeasurements[i].Value == 0 {
			pumpOnIndex = i
			break
		}
	}

	// obtaining relevant aggregate measurements
	deltaWaterLevel := levelMeasurements[0].Value - levelMeasurements[pumpOnIndex].Value
	deltaTime := flowMeasurements[0].Time.Sub(flowMeasurements[pumpOnIndex].Time).Minutes()

	changedWaterVolume := common.GetConfig().SystemConfig.TankUsableVolume *
		deltaWaterLevel / common.StaticConfig.WaterLevelUsableRegion

	avgLostFlow := changedWaterVolume / deltaTime
	if avgLostFlow > common.GetConfig().SystemConfig.WaterLeakAmount {
		common.SendNotification("Water leak detected! Average lost flow: " + strconv.FormatFloat(avgLostFlow, 'f', 1, 64) + " mL/min")
		common.LogF(common.SeverityWarning, "water leak detected! average lost flow: %f mL/min", avgLostFlow)
		timeLastSentLeakNotification = time.Now()
	}
}

var timeLastSentLightDegredationNotification time.Time

func checkForLightDegredation(readings common.SensorReadings) {
	if timeLastSentLightDegredationNotification.Add(72 * time.Hour).After(time.Now()) {
		return
	}

	// if the light just turned off, we can check to see if the light's average produced intensity is below
	// the threshold.

	lightMeasurements, err := common.GetPastSensorMeasurements(common.SensorMeasurementNames.LightIntensity, 60*time.Minute)
	if err != nil {
		common.LogF(common.SeverityWarning, "failed to check for light degredation because the database failed: %v", err)
		return
	}

	if len(lightMeasurements) < 2 {
		common.LogF(common.SeverityInfo, "not enough measurements to check for light degredation")
		return
	}

	// the light must have been on during the last minute, but just turned off
	if lightMeasurements[0].Value < common.GetConfig().SystemConfig.LightOnThreshold ||
		readings.LightIntensity > common.GetConfig().SystemConfig.LightOnThreshold {
		return
	}

	// the first measurement is the most recent
	numAcc := 0
	sum := 0.0

	for i := 0; i < len(lightMeasurements); i++ {
		if lightMeasurements[i].Value > common.GetConfig().SystemConfig.LightOnThreshold {
			numAcc++
			sum += lightMeasurements[i].Value
		}
	}

	avgLight := sum / float64(numAcc)

	degThres := common.GetConfig().SystemConfig.LightDegredationThreshold
	if avgLight < degThres {
		common.SendNotification("Light degredation detected! Down " + strconv.FormatFloat((degThres-avgLight)/degThres, 'f', 1, 64) + "% from healthy.")
		common.LogF(common.SeverityWarning, "light degredation detected! average light intensity: %f uW/cm^2 compared to %f uW/cm^2", avgLight, degThres)
		timeLastSentLightDegredationNotification = time.Now()
	}
}

var timeLastSentCO2Notification time.Time

func checkForCO2Excess(readings common.SensorReadings) {
	if timeLastSentCO2Notification.Add(72 * time.Hour).After(time.Now()) {
		return
	}

	if readings.AirCO2PPM > common.GetConfig().SystemConfig.MaximumCO2PPM {
		common.SendNotification("CO2 excess detected! Current CO2: " + strconv.FormatFloat(readings.AirCO2PPM, 'f', 1, 64) + " ppm")
		common.LogF(common.SeverityWarning, "CO2 excess detected! current CO2: %f ppm", readings.AirCO2PPM)
		timeLastSentCO2Notification = time.Now()
	} else if readings.AirCO2PPM < common.GetConfig().SystemConfig.MinimumCO2PPM {
		common.SendNotification("CO2 deficiency detected! Current CO2: " + strconv.FormatFloat(readings.AirCO2PPM, 'f', 1, 64) + " ppm")
		common.LogF(common.SeverityWarning, "CO2 deficiency detected! current CO2: %f ppm", readings.AirCO2PPM)
		timeLastSentCO2Notification = time.Now()
	}
}

var timeLastSentAirTempNotification time.Time

func checkForAirTempExcess(readings common.SensorReadings) {
	if timeLastSentAirTempNotification.Add(12 * time.Hour).After(time.Now()) {
		return
	}

	if readings.AirTemperature > common.GetConfig().SystemConfig.MaximumAirTemp {
		common.SendNotification("Air temperature excess detected! Current temperature: " + strconv.FormatFloat(readings.AirTemperature, 'f', 1, 64) + " degC")
		common.LogF(common.SeverityWarning, "air temperature excess detected! current temperature: %f degC", readings.AirTemperature)
		timeLastSentAirTempNotification = time.Now()
	} else if readings.AirTemperature < common.GetConfig().SystemConfig.MinimumAirTemp {
		common.SendNotification("Air temperature deficiency detected! Current temperature: " + strconv.FormatFloat(readings.AirTemperature, 'f', 1, 64) + " degC")
		common.LogF(common.SeverityWarning, "air temperature deficiency detected! current temperature: %f degC", readings.AirTemperature)
		timeLastSentAirTempNotification = time.Now()
	}
}

var timeLastSentAirHumidityNotification time.Time

func checkForAirHumidityExcess(readings common.SensorReadings) {
	if timeLastSentAirHumidityNotification.Add(72 * time.Hour).After(time.Now()) {
		return
	}

	if readings.AirRelativeHumidity > common.GetConfig().SystemConfig.MaximumAirHumidity {
		common.SendNotification("Air humidity excess detected! Current humidity: " + strconv.FormatFloat(readings.AirRelativeHumidity, 'f', 1, 64) + "%")
		common.LogF(common.SeverityWarning, "air humidity excess detected! current humidity: %f%%", readings.AirRelativeHumidity)
		timeLastSentAirHumidityNotification = time.Now()
	} else if readings.AirRelativeHumidity < common.GetConfig().SystemConfig.MinimumAirHumidity {
		common.SendNotification("Air humidity deficiency detected! Current humidity: " + strconv.FormatFloat(readings.AirRelativeHumidity, 'f', 1, 64) + "%")
		common.LogF(common.SeverityWarning, "air humidity deficiency detected! current humidity: %f%%", readings.AirRelativeHumidity)
		timeLastSentAirHumidityNotification = time.Now()
	}
}

var timeLastSentPHNotification time.Time

func checkForPHExcess(readings common.SensorReadings) {
	if timeLastSentPHNotification.Add(24 * time.Hour).After(time.Now()) {
		return
	}

	if readings.PH > common.GetConfig().SystemConfig.PHMaximumDeviation {
		common.SendNotification("PH excess detected! Current PH: " + strconv.FormatFloat(readings.PH, 'f', 1, 64))
		common.LogF(common.SeverityWarning, "PH excess detected! current PH: %f", readings.PH)
		timeLastSentPHNotification = time.Now()
	} else if readings.PH < common.GetConfig().SystemConfig.PHMaximumDeviation {
		common.SendNotification("PH deficiency detected! Current PH: " + strconv.FormatFloat(readings.PH, 'f', 1, 64))
		common.LogF(common.SeverityWarning, "PH deficiency detected! current PH: %f", readings.PH)
		timeLastSentPHNotification = time.Now()
	}
}

var timeLastSentECNotification time.Time

func checkForECExcess(readings common.SensorReadings) {
	if timeLastSentECNotification.Add(24 * time.Hour).After(time.Now()) {
		return
	}

	if readings.ElectricalConductivity > common.GetConfig().SystemConfig.ECMaximumDeviation {
		common.SendNotification("EC excess detected! Current EC: " + strconv.FormatFloat(readings.ElectricalConductivity, 'f', 1, 64))
		common.LogF(common.SeverityWarning, "EC excess detected! current EC: %f", readings.ElectricalConductivity)
		timeLastSentECNotification = time.Now()
	} else if readings.ElectricalConductivity < common.GetConfig().SystemConfig.ECMaximumDeviation {
		common.SendNotification("EC deficiency detected! Current EC: " + strconv.FormatFloat(readings.ElectricalConductivity, 'f', 1, 64))
		common.LogF(common.SeverityWarning, "EC deficiency detected! current EC: %f", readings.ElectricalConductivity)
		timeLastSentECNotification = time.Now()
	}
}

var timeLastSentWaterLevelNotification time.Time

func checkForLowWaterLevel(readings common.SensorReadings) {
	if timeLastSentWaterLevelNotification.Add(24 * time.Hour).After(time.Now()) {
		return
	}

	if readings.WaterLevel < common.GetConfig().SystemConfig.MinimumWaterLevel {
		common.SendNotification("Water level deficiency detected! Current water level: " + strconv.FormatFloat(readings.WaterLevel, 'f', 1, 64) + " cm")
		common.LogF(common.SeverityWarning, "water level deficiency detected! current water level: %f cm", readings.WaterLevel)
		timeLastSentWaterLevelNotification = time.Now()
	}
}
