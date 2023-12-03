package api

import (
	"encoding/json"
	"net/http"
	"strconv"

	"github.com/danielcbailey/HydroponicsProject/server/common"
)

func handleAddPlant(w http.ResponseWriter, r *http.Request) {
	if !checkAuth(w, r) {
		return
	}

	var plant common.PlantConfiguration
	err := json.NewDecoder(r.Body).Decode(&plant)
	if err != nil {
		common.LogF(common.SeverityError, "Error decoding plant from request body: %v", err)
		w.WriteHeader(http.StatusBadRequest)
		return
	}

	config := common.GetConfig()
	config.SystemConfig.PlantConfigs = append(config.SystemConfig.PlantConfigs, plant)
	err = common.SaveConfig()

	if err != nil {
		common.LogF(common.SeverityError, "Error saving plant configuration: %v", err)
		w.WriteHeader(http.StatusInternalServerError)
		return
	}

	w.WriteHeader(http.StatusOK)
}

func handleRemovePlant(w http.ResponseWriter, r *http.Request) {
	if !checkAuth(w, r) {
		return
	}

	plantID := r.URL.Query().Get("plant_id")
	plantIDInt, err := strconv.Atoi(plantID)
	if err != nil {
		common.LogF(common.SeverityError, "Error converting plant ID to int: %v", err)
		w.WriteHeader(http.StatusBadRequest)
		return
	}

	config := common.GetConfig()
	for i, p := range config.SystemConfig.PlantConfigs {
		if p.PlantID == plantIDInt {
			config.SystemConfig.PlantConfigs = append(config.SystemConfig.PlantConfigs[:i], config.SystemConfig.PlantConfigs[i+1:]...)
			err = common.SaveConfig()

			if err != nil {
				common.LogF(common.SeverityError, "Error saving plant configuration: %v", err)
				w.WriteHeader(http.StatusInternalServerError)
				return
			}

			w.WriteHeader(http.StatusOK)
			return
		}
	}

	w.WriteHeader(http.StatusBadRequest)
}

func handleUpdatePlant(w http.ResponseWriter, r *http.Request) {
	if !checkAuth(w, r) {
		return
	}

	var plant common.PlantConfiguration
	err := json.NewDecoder(r.Body).Decode(&plant)
	if err != nil {
		common.LogF(common.SeverityError, "Error decoding plant from request body: %v", err)
		w.WriteHeader(http.StatusBadRequest)
		return
	}

	config := common.GetConfig()
	for i, p := range config.SystemConfig.PlantConfigs {
		if p.PlantID == plant.PlantID {
			config.SystemConfig.PlantConfigs[i] = plant
			err = common.SaveConfig()

			if err != nil {
				common.LogF(common.SeverityError, "Error saving plant configuration: %v", err)
				w.WriteHeader(http.StatusInternalServerError)
				return
			}

			w.WriteHeader(http.StatusOK)
			return
		}
	}

	w.WriteHeader(http.StatusBadRequest)
}

func handlePlantHolderUpdate(w http.ResponseWriter, r *http.Request) {
	if !checkAuth(w, r) {
		return
	}

	var plantHolder common.PlantHolder
	err := json.NewDecoder(r.Body).Decode(&plantHolder)
	if err != nil {
		common.LogF(common.SeverityError, "Error decoding plant holder from request body: %v", err)
		w.WriteHeader(http.StatusBadRequest)
		return
	}

	config := common.GetConfig()
	for i, p := range config.SystemConfig.PlantHolders {
		if p.XCoordinate == plantHolder.XCoordinate && p.YCoordinate == plantHolder.YCoordinate {
			config.SystemConfig.PlantHolders[i] = plantHolder
			err = common.SaveConfig()

			if err != nil {
				common.LogF(common.SeverityError, "Error saving plant holder configuration: %v", err)
				w.WriteHeader(http.StatusInternalServerError)
				return
			}

			w.WriteHeader(http.StatusOK)
			return
		}
	}

	w.WriteHeader(http.StatusBadRequest)
}
