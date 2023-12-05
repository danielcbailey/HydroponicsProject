package api

import "net/http"

func AddRoutes(mux *http.ServeMux) {
	mux.HandleFunc("/api/authenticate", handleAuthenticate)

	// Configuration
	mux.HandleFunc("/api/getConfig", handleGetConfig)
	mux.HandleFunc("/api/updateSettings", handleUpdateSettings)
	mux.HandleFunc("/api/addPlant", handleAddPlant)
	mux.HandleFunc("/api/removePlant", handleRemovePlant)
	mux.HandleFunc("/api/updatePlant", handleUpdatePlant)
	mux.HandleFunc("/api/updatePlantHolder", handlePlantHolderUpdate)

	// Data
	mux.HandleFunc("/api/getPastData", handleGetData)
	mux.HandleFunc("/api/dataWebsocket", handleDataWebsocket)
	mux.HandleFunc("/api/getStateEstimate", handleStateEstimation)

	// Adjustments
	mux.HandleFunc("/api/getPHAdjustment", handleGetPHAdjustment)
	mux.HandleFunc("/api/getECAdjustment", handleGetECAdjustment)
	mux.HandleFunc("/api/getWaterAdjustment", handleWaterAdjustment)

	initAuth()
}
