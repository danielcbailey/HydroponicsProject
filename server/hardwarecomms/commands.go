package hardwarecomms

import (
	"fmt"
	"time"

	"github.com/danielcbailey/HydroponicsProject/server/common"
)

func (hc *HardwareComms) SetTime(t time.Time) error {
	unixTime := t.Unix()
	unixTime += int64(common.GetConfig().TimezoneOffset * 60)

	_, err := hc.sendCommand("setTime", commandSetTime{UnixTime: unixTime})
	return err
}

func (hc *HardwareComms) SetSchedule(schedule common.Schedule) error {
	_, err := hc.sendCommand("setSchedule", schedule)
	return err
}

func (hc *HardwareComms) SetPumpState(on bool) error {
	_, err := hc.sendCommand("setPumpState", commandSetBooleanState{State: on})
	return err
}

func (hc *HardwareComms) SetLightState(on bool) error {
	_, err := hc.sendCommand("setLightState", commandSetBooleanState{State: on})
	return err
}

func (hc *HardwareComms) GetSensors() (common.SensorReadings, error) {
	response, err := hc.sendCommand("getSensors", commandBlank{})
	if err != nil {
		return common.SensorReadings{}, err
	}

	sensors, ok := response.(common.SensorReadings)
	if !ok {
		return common.SensorReadings{}, fmt.Errorf("COMMUNICATION_FAILURE: Invalid response format for get sensors: %v", response)
	}

	return sensors, nil
}
