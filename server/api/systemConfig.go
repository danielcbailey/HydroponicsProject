package api

import (
	"encoding/json"
	"net/http"

	"github.com/danielcbailey/HydroponicsProject/server/common"
)

func handleGetConfig(w http.ResponseWriter, r *http.Request) {
	if !checkAuth(w, r) {
		return
	}

	config := common.GetConfig().SystemConfig
	json.NewEncoder(w).Encode(config)
}

func handleUpdateSettings(w http.ResponseWriter, r *http.Request) {
	if !checkAuth(w, r) {
		return
	}

	var settings common.SystemConfiguration
	err := json.NewDecoder(r.Body).Decode(&settings)
	if err != nil {
		common.LogF(common.SeverityError, "Error decoding settings from request body: %v", err)
		w.WriteHeader(http.StatusBadRequest)
		return
	}

	config := common.GetConfig()
	settings.PlantConfigs = config.SystemConfig.PlantConfigs
	settings.PlantHolders = config.SystemConfig.PlantHolders
	config.SystemConfig = settings
	err = common.SaveConfig()

	if err != nil {
		common.LogF(common.SeverityError, "Error saving settings: %v", err)
		w.WriteHeader(http.StatusInternalServerError)
		return
	}

	w.WriteHeader(http.StatusOK)
}
