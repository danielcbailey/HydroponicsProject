package common

import (
	"os"

	"gopkg.in/yaml.v3"
)

type PlantConfiguration struct {
	PlantID                 int     `yaml:"plant_id" json:"plant_id"`
	Name                    string  `yaml:"name" json:"name"`
	ECLowerLimit            float64 `yaml:"ec_lower_limit" json:"ec_lower_limit"` // in mS/cm
	ECUpperLimit            float64 `yaml:"ec_upper_limit" json:"ec_upper_limit"` // in mS/cm
	PHLowerLimit            float64 `yaml:"ph_lower_limit" json:"ph_lower_limit"` // in `yaml:"ph_lower_limit" json:"ph_lower_limit"
	PHUpperLimit            float64 `yaml:"ph_upper_limit" json:"ph_upper_limit"` // in `yaml:"ph_upper_limit" json:"ph_upper_limit"`
	LightHoursLowerLimit    float64 `yaml:"light_hours_lower_limit" json:"light_hours_lower_limit"`
	LightHoursUpperLimit    float64 `yaml:"light_hours_upper_limit" json:"light_hours_upper_limit"`
	TransplantToHarvestDays int     `yaml:"transplant_to_harvest_days" json:"transplant_to_harvest_days"`
	HarvestLengthDays       int     `yaml:"harvest_length_days" json:"harvest_length_days"`
	Color1                  string  `yaml:"color1" json:"color1"` // hex color code
	Color2                  string  `yaml:"color2" json:"color2"` // hex color code
}

type PlantHolder struct {
	XCoordinate      int   `yaml:"x_coordinate" json:"x_coordinate"`
	YCoordinate      int   `yaml:"y_coordinate" json:"y_coordinate"`
	PlantID          int   `yaml:"plant_id" json:"plant_id"`
	TimeTransplanted int64 `yaml:"time_transplanted" json:"time_transplanted"` // unix time
}

type SystemConfiguration struct {
	// configuration
	PlantConfigs []PlantConfiguration `yaml:"plant_configs" json:"plant_configs"`
	PlantHolders []PlantHolder        `yaml:"plant_holders" json:"plant_holders"`

	// thresholds
	WaterLeakAmount           float64 `yaml:"water_leak_amount" json:"water_leak_amount"`                     // threshold for water leak detection, in mL/min
	LightDegredationThreshold float64 `yaml:"light_degredation_threshold" json:"light_degredation_threshold"` // in uW/cm^2
	LightOnThreshold          float64 `yaml:"light_on_threshold" json:"light_on_threshold"`                   // in uW/cm^2
	MinimumWaterLevel         float64 `yaml:"minimum_water_level" json:"minimum_water_level"`                 // in inches
	MinimumCO2PPM             float64 `yaml:"minimum_co2_ppm" json:"minimum_co2_ppm"`                         // in ppm
	MaximumCO2PPM             float64 `yaml:"maximum_co2_ppm" json:"maximum_co2_ppm"`                         // in ppm
	MinimumAirTemp            float64 `yaml:"minimum_air_temp" json:"minimum_air_temp"`                       // in degC
	MaximumAirTemp            float64 `yaml:"maximum_air_temp" json:"maximum_air_temp"`                       // in degC
	MinimumAirHumidity        float64 `yaml:"minimum_air_humidity" json:"minimum_air_humidity"`               // in %
	MaximumAirHumidity        float64 `yaml:"maximum_air_humidity" json:"maximum_air_humidity"`               // in %
	PHMaximumDeviation        float64 `yaml:"ph_maximum_deviation" json:"ph_maximum_deviation"`               // in pH
	ECMaximumDeviation        float64 `yaml:"ec_maximum_deviation" json:"ec_maximum_deviation"`               // in mS/cm
	LightMaximumDeviation     float64 `yaml:"light_maximum_deviation" json:"light_maximum_deviation"`         // in lux

	// physical specifications
	TankUsableVolume float64 `yaml:"tank_usable_volume" json:"tank_usable_volume"` // in L
	TankTotalVolume  float64 `yaml:"tank_total_volume" json:"tank_total_volume"`   // in L
	LightWattage     float64 `yaml:"light_wattage" json:"light_wattage"`           // in W
	PumpWattage      float64 `yaml:"pump_wattage" json:"pump_wattage"`             // in W

	// constants for assisting user in adjustments
	ECPerVolNutrient     float64 `yaml:"ec_per_vol_nutrient" json:"ec_per_vol_nutrient"`           // in (mS/cm)/mL/L (mS/cm = delta EC, mL = volume of nutrient solution, L = volume of water)
	PHUpPerVolSolution   float64 `yaml:"ph_up_per_vol_solution" json:"ph_up_per_vol_solution"`     // in pH/mL/L
	PHDownPerVolSolution float64 `yaml:"ph_down_per_vol_solution" json:"ph_down_per_vol_solution"` // in pH/mL/L

	// Alerting
	AlertingEnabled bool     `yaml:"alerting_enabled" json:"alerting_enabled"`
	PhoneNumbers    []string `yaml:"phone_numbers" json:"phone_numbers"`
}

type Config struct {
	TimezoneOffset       int                 `yaml:"timezone_offset_mins"`
	ClickhouseConnString string              `yaml:"clickhouse_conn_string"`
	TwilioAccountSID     string              `yaml:"twilio_account_sid"`
	TwilioAuthToken      string              `yaml:"twilio_auth_token"`
	SerialFilePath       string              `yaml:"serial_file_path"`
	LogFileDir           string              `yaml:"log_file_dir"`
	LogRetentionDays     int                 `yaml:"log_retention_days"`
	SystemPassword       string              `yaml:"system_password"`
	SystemConfig         SystemConfiguration `yaml:"system_configuration"`
}

var config *Config

func GetConfig() *Config {
	if config == nil {
		config = &Config{}
		configFile, err := os.ReadFile("config.yaml")
		if err != nil {
			panic(err)
		}

		err = yaml.Unmarshal([]byte(configFile), config)
		if err != nil {
			panic(err)
		}
	}

	return config
}

func SaveConfig() error {
	configFile, err := yaml.Marshal(config)
	if err != nil {
		return err
	}

	err = os.WriteFile("config.yaml", configFile, 0644)
	if err != nil {
		return err
	}

	return nil
}

type StaticConfiguration struct {
	WaterLevelUsableRegion float64
}

var StaticConfig = StaticConfiguration{
	WaterLevelUsableRegion: 6, // six inches, the bottom 2 inches of the 8" sensor are not usable
}
