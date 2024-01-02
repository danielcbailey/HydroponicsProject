package asynchronousactivities

import (
	"errors"
	"strings"
	"sync"
	"time"

	"github.com/danielcbailey/HydroponicsProject/server/common"
	"github.com/danielcbailey/HydroponicsProject/server/hardwarecomms"
)

var commMutex sync.Mutex
var comm *hardwarecomms.HardwareComms
var lastSensorReadings common.SensorReadings

var listenerMutex sync.Mutex
var listeners []chan common.SensorReadings

func RunAsyncActivities(comms *hardwarecomms.HardwareComms) {
	commMutex.Lock()
	comm = comms
	comm.SetTime(time.Now())
	comm.SetSchedule(common.GetSchedule())
	comm.ReconnectHandler = onReconnect
	comm.NotificationHandler = onNotification
	commMutex.Unlock()

	var err error
	i := 0
	j := 0
	for {
		commMutex.Lock()
		lastSensorReadings, err = comm.GetSensors()
		commMutex.Unlock()

		if err != nil {
			common.LogF(common.SeverityError, "Error getting sensor readings: %v", err)
		} else {
			// save to database, perform analysis, and update listeners
			performAnalysis(lastSensorReadings)
			err := common.AddSensorReadings(lastSensorReadings)
			if err != nil {
				common.LogF(common.SeverityError, "Error saving sensor readings to database: %v", err)
			}
			listenerMutex.Lock()
			for _, listener := range listeners {
				listener <- lastSensorReadings
			}
			listenerMutex.Unlock()
		}

		i++
		if i == 60 {
			// once per hour, send current time to hardware
			commMutex.Lock()
			err = comm.SetTime(time.Now())
			commMutex.Unlock()
			i = 0
		}
		j++
		if j == 60*24 {
			commMutex.Lock()
			comm.SetSchedule(common.GetSchedule())
			commMutex.Unlock()
			j = 0
		}

		time.Sleep(1 * time.Minute)
	}
}

func onReconnect() {
	commMutex.Lock()
	comm.SetTime(time.Now())
	comm.SetSchedule(common.GetSchedule())
	commMutex.Unlock()
}

func onNotification(messageType string, message interface{}) {
	if messageType == "event/log" {
		logEntry, ok := message.(hardwarecomms.LogNotification)
		if ok {
			common.LogF(strings.ToUpper(logEntry.Severity), logEntry.Message)
		}
	}
}

func RequestPumpState(on bool) error {
	commMutex.Lock()
	defer commMutex.Unlock()

	if comm == nil {
		return errors.New("COMMUNICATION_FAILURE: Hardware communications not initialized")
	}

	err := comm.SetPumpState(on)
	if err != nil {
		return err
	}

	lastSensorReadings.PumpOn = on

	return nil
}

func RequestLightState(on bool) error {
	commMutex.Lock()
	defer commMutex.Unlock()

	if comm == nil {
		return errors.New("COMMUNICATION_FAILURE: Hardware communications not initialized")
	}

	err := comm.SetLightState(on)
	if err != nil {
		return err
	}

	lastSensorReadings.LightOn = on

	return nil
}

func GetLastSensorReadings() common.SensorReadings {
	return lastSensorReadings
}

func RequestSetSchedule(schedule common.Schedule) error {
	commMutex.Lock()
	defer commMutex.Unlock()

	if comm == nil {
		return errors.New("COMMUNICATION_FAILURE: Hardware communications not initialized")
	}

	return comm.SetSchedule(schedule)
}

func AddReadingsListener(listener chan common.SensorReadings) {
	listenerMutex.Lock()
	listeners = append(listeners, listener)
	listenerMutex.Unlock()
}

func RemoveReadingsListener(listener chan common.SensorReadings) {
	listenerMutex.Lock()
	for i, l := range listeners {
		if l == listener {
			listeners = append(listeners[:i], listeners[i+1:]...)
			break
		}
	}
	listenerMutex.Unlock()
}
