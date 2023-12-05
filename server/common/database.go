package common

import (
	"database/sql"
	"errors"
	"time"

	_ "github.com/ClickHouse/clickhouse-go"
)

var db *sql.DB

func SetUpDatabase() error {
	var err error
	db, err = sql.Open("clickhouse", GetConfig().ClickhouseConnString)
	return err
}

type sensorMeasurementNames struct {
	AirTemperature         string
	AirRelativeHumidity    string
	AirCO2PPM              string
	WaterLevel             string
	CurrentLiquidFlow      string
	CumulativeLiquidFlow   string
	LightIntensity         string
	PH                     string
	ElectricalConductivity string
}

var SensorMeasurementNames = sensorMeasurementNames{
	AirTemperature:         "air_temp",
	AirRelativeHumidity:    "air_rel_humidity",
	AirCO2PPM:              "air_co2",
	WaterLevel:             "water_level",
	CurrentLiquidFlow:      "water_flow",
	CumulativeLiquidFlow:   "water_acc_flow",
	LightIntensity:         "light_level",
	PH:                     "ph",
	ElectricalConductivity: "electrical_conductivity",
}

func AddSensorReadings(readings SensorReadings) error {
	tx, err := db.Begin()
	if err != nil {
		return err
	}
	_, err = tx.Exec(
		"INSERT INTO sensor_readings (time, air_temp, air_rel_humidity, air_co2, water_level, water_flow, water_acc_flow, light_level, ph, electrical_conductivity) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)",
		time.Now(),
		readings.AirTemperature,
		readings.AirRelativeHumidity,
		readings.AirCO2PPM,
		readings.WaterLevel,
		readings.CurrentLiquidFlow,
		readings.CumulativeLiquidFlow,
		readings.LightIntensity,
		readings.PH,
		readings.ElectricalConductivity,
	)

	if err != nil {
		tx.Rollback()
	} else {
		err = tx.Commit()
	}

	return err
}

func GetPastSensorMeasurements(measurement string, timeRange time.Duration) ([]SensorMeasurement, error) {
	rows, err := db.Query("SELECT time, "+measurement+" FROM sensor_readings WHERE time > ? ORDER BY time DESC", time.Now().Add(-timeRange).Unix())
	if err != nil {
		return nil, err
	}
	defer rows.Close()

	measurements := []SensorMeasurement{}
	for rows.Next() {
		var time time.Time
		var value float64
		err := rows.Scan(&time, &value)
		if err != nil {
			return nil, err
		}

		measurements = append(measurements, SensorMeasurement{Time: time, Value: value, UnixTime: time.Unix()})
	}

	return measurements, nil
}

func GetPastSensorMeasurementsWithResolution(measurement string, timeRange time.Duration, resolution time.Duration) ([]SensorMeasurement, error) {
	// if measurement contains characters other than a-z, A-Z, 0-9, and _, should error
	valid := true
	for _, c := range measurement {
		if !(c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c >= '0' && c <= '9' || c == '_') {
			valid = false
			break
		}
	}
	if !valid {
		return nil, errors.New("invalid measurement name")
	}

	rows, err := db.Query("SELECT toStartOfInterval(time, INTERVAL ? MINUTE) AS iTime, avg("+measurement+") AS iValue FROM sensor_readings WHERE time > timestamp(?) GROUP BY iTime ORDER BY iTime DESC", resolution.Minutes(), time.Now().Add(-timeRange).Unix())
	if err != nil {
		return nil, err
	}
	defer rows.Close()

	measurements := []SensorMeasurement{}
	for rows.Next() {
		var time time.Time
		var value float64
		err := rows.Scan(&time, &value)
		if err != nil {
			return nil, err
		}

		measurements = append(measurements, SensorMeasurement{Time: time, Value: value, UnixTime: time.Unix()})
	}

	return measurements, nil
}
