package common

import "time"

type ScheduleEntry struct {
	EventTime int    `json:"event_time"`
	EventType string `json:"event_type"`
}

type Schedule struct {
	Schedule []ScheduleEntry `json:"schedule"`
}

type SensorReadings struct {
	WaterLevel             float64 `json:"water_level"`
	CumulativeLiquidFlow   float64 `json:"cumulative_liquid_flow"`
	CurrentLiquidFlow      float64 `json:"current_liquid_flow"`
	LightIntensity         float64 `json:"light_intensity"`
	PH                     float64 `json:"pH"`
	ElectricalConductivity float64 `json:"electrical_conductivity"`
	AirTemperature         float64 `json:"air_temperature"`
	AirRelativeHumidity    float64 `json:"air_relative_humidity"`
	AirCO2PPM              float64 `json:"air_co2_ppm"`
	LightOn                bool    `json:"light"`
	PumpOn                 bool    `json:"pump"`
}

type SensorMeasurement struct {
	Time     time.Time `json:"-"`
	UnixTime int64     `json:"time"`
	Value    float64   `json:"value"`
}
