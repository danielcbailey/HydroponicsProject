package asynchronousactivities

import (
	"errors"
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
	commMutex.Unlock()

	var err error
	i := 0
	for {
		commMutex.Lock()
		lastSensorReadings, err = comm.GetSensors()
		commMutex.Unlock()

		if err != nil {
			common.LogF(common.SeverityError, "Error getting sensor readings: %v", err)
		} else {
			// save to database, perform analysis, and update listeners
			performAnalysis(lastSensorReadings)
			common.AddSensorReadings(lastSensorReadings)
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

		time.Sleep(1 * time.Minute)
	}
}

func RequestPumpState(on bool) error {
	commMutex.Lock()
	defer commMutex.Unlock()

	if comm == nil {
		return errors.New("COMMUNICATION_FAILURE: Hardware communications not initialized")
	}

	return comm.SetPumpState(on)
}

func RequestLightState(on bool) error {
	commMutex.Lock()
	defer commMutex.Unlock()

	if comm == nil {
		return errors.New("COMMUNICATION_FAILURE: Hardware communications not initialized")
	}

	return comm.SetLightState(on)
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
