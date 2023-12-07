package main

import (
	"net/http"

	"github.com/danielcbailey/HydroponicsProject/server/api"
	"github.com/danielcbailey/HydroponicsProject/server/asynchronousactivities"
	"github.com/danielcbailey/HydroponicsProject/server/common"
	"github.com/danielcbailey/HydroponicsProject/server/hardwarecomms"
)

func main() {
	// initialize communication with hardware
	hComm, err := hardwarecomms.OpenComms(common.GetConfig().SerialFilePath)
	if err != nil {
		common.LogF(common.SeverityFatal, "Error opening communication with hardware: %v", err)
		return
	}

	err = common.SetUpDatabase()
	if err != nil {
		common.LogF(common.SeverityFatal, "Error setting up database: %v", err)
		return
	}

	go asynchronousactivities.RunAsyncActivities(hComm)

	// start web server
	common.LogF(common.SeverityInfo, "Starting web server on port 8088")

	api.AddRoutes(http.DefaultServeMux)

	fs := http.FileServer(http.Dir("./publicStatic"))
	http.DefaultServeMux.Handle("/", fs)

	err = http.ListenAndServe(":8088", http.DefaultServeMux)
	if err != nil {
		common.LogF(common.SeverityFatal, "Error starting web server: %v", err)
		return
	}
}
