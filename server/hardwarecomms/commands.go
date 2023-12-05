package hardwarecomms

import (
	"encoding/json"
	"fmt"
	"time"

	"github.com/danielcbailey/HydroponicsProject/server/common"
)

func (hc *HardwareComms) SetTime(t time.Time) error {
	unixTime := t.Unix()
	unixTime += int64(common.GetConfig().TimezoneOffset * 60)

	_, err := hc.sendCommand("config/settime", commandSetTime{UnixTime: unixTime})
	return err
}

func (hc *HardwareComms) SetSchedule(schedule common.Schedule) error {
	common.LogF(common.SeverityInfo, "Sending schedule to hardware: %v", schedule)
	_, err := hc.sendCommand("config/setschedule", schedule)
	return err
}

func (hc *HardwareComms) SetPumpState(on bool) error {
	_, err := hc.sendCommand("controls/setpump", commandSetBooleanState{State: on})
	return err
}

func (hc *HardwareComms) SetLightState(on bool) error {
	_, err := hc.sendCommand("sensors/setlight", commandSetBooleanState{State: on})
	return err
}

func (hc *HardwareComms) GetSensors() (common.SensorReadings, error) {
	response, err := hc.sendCommand("sensors/read", commandBlank{})
	if err != nil {
		return common.SensorReadings{}, err
	}

	sensors := common.SensorReadings{}
	err = json.Unmarshal(response, &sensors)

	if err != nil {
		return common.SensorReadings{}, fmt.Errorf("COMMUNICATION_FAILURE: Invalid response format for get sensors: %v", response)
	}

	return sensors, nil
}
