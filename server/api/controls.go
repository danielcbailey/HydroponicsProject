package api

import (
	"net/http"

	"github.com/danielcbailey/HydroponicsProject/server/asynchronousactivities"
	"github.com/danielcbailey/HydroponicsProject/server/common"
)

func handleTogglePump(w http.ResponseWriter, r *http.Request) {
	if !checkAuth(w, r) {
		return
	}

	readings := asynchronousactivities.GetLastSensorReadings()
	err := asynchronousactivities.RequestPumpState(!readings.PumpOn)
	if err != nil {
		common.LogF(common.SeverityError, "Error requesting pump state: %v", err)
		w.WriteHeader(http.StatusInternalServerError)
		return
	}

	w.WriteHeader(http.StatusOK)
}

func handleToggleLight(w http.ResponseWriter, r *http.Request) {
	if !checkAuth(w, r) {
		return
	}

	readings := asynchronousactivities.GetLastSensorReadings()
	err := asynchronousactivities.RequestLightState(!readings.LightOn)
	if err != nil {
		common.LogF(common.SeverityError, "Error requesting light state: %v", err)
		w.WriteHeader(http.StatusInternalServerError)
		return
	}

	w.WriteHeader(http.StatusOK)
}
