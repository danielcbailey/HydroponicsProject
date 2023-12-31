package api

import (
	"encoding/json"
	"net/http"

	"github.com/danielcbailey/HydroponicsProject/server/common"
)

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
