package api

import (
	"encoding/json"
	"net/http"
	"time"

	"github.com/danielcbailey/HydroponicsProject/server/common"
)

type FutureStateResponse struct {
	ECTime    int64 `json:"ec_time"` // time in seconds until EC needs to be adjusted
	PHTime    int64 `json:"ph_time"`
	WaterTime int64 `json:"water_time"`
}

// returns the estimated time until the specified measurement reaches either endpoint of the range
// min and max are the endpoints of the range, if one is zero, then it will only estimate until the other endpoint
func estimateMeasurementTime(measurementName string, min, max float64) (time.Duration, error) {
	// for now, will just use the start and end points of the last 7 days of data, but ideally
	// would fit a least squares regression to the data and use that to estimate the time
	readings, err := common.GetPastSensorMeasurementsWithResolution(measurementName, 7*24*time.Hour, time.Hour)
	if err != nil {
		return 0, err
	}

	lastValue := readings[0].Value
	oldestValue := readings[len(readings)-1].Value

	slope := (lastValue - oldestValue) / 7
	if slope > 0 && max != 0 {
		return time.Duration((max - lastValue) / slope * 24 * float64(time.Hour)), nil
	} else if slope < 0 && min != 0 {
		return time.Duration((min - lastValue) / slope * 24 * float64(time.Hour)), nil
	} else {
		return 0, nil
	}
}

func handleStateEstimation(w http.ResponseWriter, r *http.Request) {
	if !checkAuth(w, r) {
		return
	}

	// estimates when the pH, EC, and water level will reach the point of needing adjustment
	// factors in the last 7 days of data (1 hour resolution)
	ecMin, _ := common.GetTargetECRange()
	ecTime, err := estimateMeasurementTime(common.SensorMeasurementNames.ElectricalConductivity, ecMin, 0)
	if err != nil {
		common.LogF(common.SeverityError, "Error estimating EC time: %v", err)
		w.WriteHeader(http.StatusInternalServerError)
		return
	}

	phMin, phMax := common.GetTargetPHRange()
	phTime, err := estimateMeasurementTime(common.SensorMeasurementNames.PH, phMin, phMax)
	if err != nil {
		common.LogF(common.SeverityError, "Error estimating pH time: %v", err)
		w.WriteHeader(http.StatusInternalServerError)
		return
	}

	waterTime, err := estimateMeasurementTime(common.SensorMeasurementNames.WaterLevel, 8-common.StaticConfig.WaterLevelUsableRegion, 0)
	if err != nil {
		common.LogF(common.SeverityError, "Error estimating water time: %v", err)
		w.WriteHeader(http.StatusInternalServerError)
		return
	}

	response := FutureStateResponse{
		ECTime:    int64(ecTime.Seconds()),
		PHTime:    int64(phTime.Seconds()),
		WaterTime: int64(waterTime.Seconds()),
	}

	json.NewEncoder(w).Encode(response)
}
