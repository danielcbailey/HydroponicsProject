package api

import (
	"encoding/json"
	"net/http"

	"github.com/danielcbailey/HydroponicsProject/server/asynchronousactivities"
	"github.com/danielcbailey/HydroponicsProject/server/common"
)

type AdjustmentResponse struct {
	NutrientML float64 `json:"nutrient_ml"`
	PHUpML     float64 `json:"ph_up_ml"` // ml of pH up solution to add
	PHDownML   float64 `json:"ph_down_ml"`
	WaterL     float64 `json:"water_l"`
}

type TargetsResponse struct {
	ECMin    float64 `json:"ec_min"`
	ECMax    float64 `json:"ec_max"`
	PHMin    float64 `json:"ph_min"`
	PHMax    float64 `json:"ph_max"`
	LightMin float64 `json:"light_min"`
	LightMax float64 `json:"light_max"`
}

// returns the amount of liters of water in the tank
func getTankWaterVolume() float64 {
	return asynchronousactivities.GetLastSensorReadings().WaterLevel * common.GetConfig().SystemConfig.TankTotalVolume / 8
}

func handleGetPHAdjustment(w http.ResponseWriter, r *http.Request) {
	if !checkAuth(w, r) {
		return
	}

	lowerTarget, upperTarget := common.GetTargetPHRange()

	reading := asynchronousactivities.GetLastSensorReadings()

	deltaPH := (upperTarget+lowerTarget)/2 - reading.PH
	response := AdjustmentResponse{}

	if reading.WaterLevel == 0 {
		// instrumentation error, can't adjust pH
		response.PHUpML = 0
	} else if deltaPH > 0 {
		response.PHUpML = deltaPH * getTankWaterVolume() / common.GetConfig().SystemConfig.PHUpPerVolSolution
	} else {
		response.PHDownML = -deltaPH * getTankWaterVolume() / common.GetConfig().SystemConfig.PHDownPerVolSolution
	}

	json.NewEncoder(w).Encode(response)
}

func handleGetECAdjustment(w http.ResponseWriter, r *http.Request) {
	if !checkAuth(w, r) {
		return
	}

	// unlike pH, EC should be adjusted to the high end of the range, because
	// the plants will slowly lower the EC over time.
	_, upperTarget := common.GetTargetECRange()

	reading := asynchronousactivities.GetLastSensorReadings()

	deltaEC := upperTarget*1000 - reading.ElectricalConductivity
	response := AdjustmentResponse{
		NutrientML: 0,
	}

	if reading.WaterLevel != 0 {
		response.NutrientML = deltaEC * getTankWaterVolume() / (common.GetConfig().SystemConfig.ECPerVolNutrient * 1000)
	}

	json.NewEncoder(w).Encode(response)
}

func handleWaterAdjustment(w http.ResponseWriter, r *http.Request) {
	if !checkAuth(w, r) {
		return
	}

	// the adjustment should be to bring the tank back to being full
	reading := asynchronousactivities.GetLastSensorReadings()
	response := AdjustmentResponse{}

	response.WaterL = common.GetConfig().SystemConfig.TankTotalVolume - getTankWaterVolume()
	dilutionFactor := getTankWaterVolume() / common.GetConfig().SystemConfig.TankTotalVolume

	ecEff := reading.ElectricalConductivity * dilutionFactor
	phEff := reading.PH * dilutionFactor

	_, upperTarget := common.GetTargetECRange()
	deltaEC := upperTarget - ecEff
	response.NutrientML = deltaEC * getTankWaterVolume() / (common.GetConfig().SystemConfig.ECPerVolNutrient * 1000)

	lowerTarget, upperTarget := common.GetTargetPHRange()
	deltaPH := (upperTarget+lowerTarget)/2 - phEff

	if deltaPH > 0 {
		response.PHUpML = deltaPH * getTankWaterVolume() / common.GetConfig().SystemConfig.PHUpPerVolSolution
	} else {
		response.PHDownML = -deltaPH * getTankWaterVolume() / common.GetConfig().SystemConfig.PHDownPerVolSolution
	}

	json.NewEncoder(w).Encode(response)
}

func handleGetTargets(w http.ResponseWriter, r *http.Request) {
	if !checkAuth(w, r) {
		return
	}

	targets := TargetsResponse{}
	targets.ECMin, targets.ECMax = common.GetTargetECRange()
	targets.PHMin, targets.PHMax = common.GetTargetPHRange()
	targets.LightMin, targets.LightMax = common.GetTargetLightHoursRange()

	json.NewEncoder(w).Encode(targets)
}
