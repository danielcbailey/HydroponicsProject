package api

import (
	"encoding/json"
	"net/http"
	"strconv"
	"time"

	"github.com/danielcbailey/HydroponicsProject/server/asynchronousactivities"
	"github.com/danielcbailey/HydroponicsProject/server/common"
	"github.com/gorilla/websocket"
)

var upgrader = websocket.Upgrader{
	ReadBufferSize:  1024,
	WriteBufferSize: 1024,
	CheckOrigin:     func(r *http.Request) bool { return true },
}

// returns 60 data points of the specified measurement
func handleGetData(w http.ResponseWriter, r *http.Request) {
	if !checkAuth(w, r) {
		return
	}

	measurementName := r.URL.Query().Get("measurement")
	resolution := r.URL.Query().Get("resolution") // in minutes, default 1
	resolutionInt, err := strconv.Atoi(resolution)
	if err != nil {
		resolutionInt = 1
	}

	var measurements []common.SensorMeasurement
	if resolutionInt <= 1 {
		measurements, err = common.GetPastSensorMeasurements(measurementName, 60*time.Minute)
	} else {
		measurements, err = common.GetPastSensorMeasurementsWithResolution(
			measurementName,
			time.Duration(resolutionInt)*60*time.Minute,
			time.Duration(resolutionInt)*time.Minute)
	}

	if err != nil {
		common.LogF(common.SeverityError, "Error getting sensor measurements: %v", err)
		w.WriteHeader(http.StatusInternalServerError)
		return
	}

	json.NewEncoder(w).Encode(measurements)
}

func handleDataWebsocket(w http.ResponseWriter, r *http.Request) {
	if !checkAuth(w, r) {
		return
	}

	ws, err := upgrader.Upgrade(w, r, nil)
	if err != nil {
		common.LogF(common.SeverityError, "Error upgrading websocket: %v", err)
		w.WriteHeader(http.StatusInternalServerError)
		return
	}
	defer ws.Close()

	// sending a copy of the past data to the client
	pastData := asynchronousactivities.GetLastSensorReadings()
	err = ws.WriteJSON(pastData)
	if err != nil {
		common.LogF(common.SeverityError, "Error writing to websocket: %v", err)
		w.WriteHeader(http.StatusInternalServerError)
		return
	}

	listener := make(chan common.SensorReadings, 1)

	asynchronousactivities.AddReadingsListener(listener)
	for {
		newestData := <-listener
		// send data to client
		err = ws.WriteJSON(newestData)
		if err != nil {
			common.LogF(common.SeverityError, "Error writing to websocket: %v", err)
			break
		}
	}

	asynchronousactivities.RemoveReadingsListener(listener)
}
