import React from 'react';
import {httpGet, httpPost, reportInvalidToken} from "../util";
import {SettingsTextBox} from "./settingsTextBox";
import {PlantConfigurationSettings} from "./plantConfigurationSettings";

let settingsBlockStyle = {
    display: 'flex',
    flexDirection: 'row',
    alignItems: 'flex-start',
    marginTop: 30,
    marginLeft: 40,
};

let settingsSubBlockStyle = {
    display: 'flex',
    flexDirection: 'column',
    alignItems: 'flex-start',
    gap: 10,
    marginLeft: 30,
    marginTop: 4,
};

export class SettingsPage extends React.Component {
    constructor(props) {
        super(props);

        this.componentDidMount = this.componentDidMount.bind(this);
        this.addPlantConfig = this.addPlantConfig.bind(this);
        this.deletePlantConfig = this.deletePlantConfig.bind(this);
        this.areChangesPresent = this.areChangesPresent.bind(this);
        this.saveChanges = this.saveChanges.bind(this);

        this.state = {config: null, errors: {plant_configs: []}};
    }

    componentDidMount() {
        // fetching config
        httpGet('/api/getConfig').then((response) => {
            if (response.status === 200) {
                response.json().then((data) => {
                    let pConfigErrors = [];
                    for (let i = 0; i < data.plant_configs.length; i++) {
                        pConfigErrors.push({});
                    }

                    let configCopy = JSON.parse(JSON.stringify(data));
                    this.setState({config: data, originalConfig: configCopy, errors: {plant_configs: pConfigErrors}});
                });
            } else if (response.status === 401) {
                console.log("Not authorized");
                reportInvalidToken();
            } else {
                console.log("Error fetching config: " + response.status);
            }
        });
    }

    addPlantConfig() {
        let config = this.state.config;
        let errors = this.state.errors;
        // getting current maximum plant id
        let maxPlantId = 0;
        for (let i = 0; i < config.plant_configs.length; i++) {
            if (config.plant_configs[i].plant_id > maxPlantId) {
                maxPlantId = config.plant_configs[i].plant_id;
            }
        }

        config.plant_configs.push({
            plant_id: maxPlantId + 1,
            name: "New Plant",
            ec_lower_limit: 2.6,
            ec_upper_limit: 3.4,
            ph_lower_limit: 6.2,
            ph_upper_limit: 7.1,
            light_hours_lower_limit: 12,
            light_hours_upper_limit: 16,
            transplant_to_harvest_days: 60,
            harvest_length_days: 34,
            color1: "#49cc64"
        });

        errors.plant_configs.push({});
        this.setState({config: config, errors: errors, saveError: null});
    }

    deletePlantConfig(index) {
        // must make sure that no plant holders are using the plant config
        let plantConfig = this.state.config.plant_configs[index];
        let plantHolders = this.state.config.plant_holders;
        let plantConfigUsed = false;
        for (let i = 0; i < plantHolders.length; i++) {
            if (plantHolders[i].plant_id === plantConfig.plant_id) {
                plantConfigUsed = true;
                break;
            }
        }

        if (plantConfigUsed) {
            let errors = this.state.errors;
            errors.plant_configs[index].delete = "Cannot delete plant config while it is in use.";
            this.setState({errors: errors});
            return;
        }

        if (this.state.config.plant_configs.length === 1) {
            let errors = this.state.errors;
            errors.plant_configs[index].delete = "Must have at least one plant config.";
            this.setState({errors: errors});
            return;
        }

        // deleting plant config
        let config = this.state.config;
        config.plant_configs.splice(index, 1);
        let errors = this.state.errors;
        errors.plant_configs.splice(index, 1);
        this.setState({config: config, errors: errors});
    }

    areChangesPresent() {
        // comparing config to originalConfig
        if (this.state.config == null || this.state.originalConfig == null) {
            return false;
        }

        if (this.state.config.plant_configs.length !== this.state.originalConfig.plant_configs.length) {
            console.log("Plant config length changed");
            return true;
        }

        for (let i = 0; i < this.state.config.plant_configs.length; i++) {
            let configChanged = false;
            Object.keys(this.state.config.plant_configs[i]).forEach((key) => {
                if (this.state.config.plant_configs[i][key] !== this.state.originalConfig.plant_configs[i][key]) {
                    console.log("Plant config changed: " + key);
                    configChanged = true;
                }
            });

            if (configChanged) {
                return true;
            }
        }

        let configChanged = false;
        Object.keys(this.state.config).forEach((key) => {
            if (key !== "plant_configs" && key !== "plant_holders" && key !== "phone_numbers" && this.state.config[key] !== this.state.originalConfig[key]) {
                console.log("Config changed: " + key);
                configChanged = true;
            }
        });

        return configChanged;
    }

    errorsPresent() {
        // checking for any errors
        if (this.state.errors == null) {
            return false;
        }

        // checking plant configs
        for (let i = 0; i < this.state.errors.plant_configs.length; i++) {
            if (Object.keys(this.state.errors.plant_configs[i]).length === 1 && this.state.errors.plant_configs[i].delete != null) {
                delete this.state.errors.plant_configs[i].delete;
            } else if (Object.keys(this.state.errors.plant_configs[i]).length > 0) {
                return true;
            }
        }

        if (Object.keys(this.state.errors).length === 1) {
            return false; // only plant_configs
        }

        return true;
    }

    saveChanges() {
        // checking for any errors
        if (this.errorsPresent()) {
            this.setState({saveError: "Cannot save changes while errors are present."});
            return;
        }

        // sending config to the server
        httpPost('/api/updateSettings', JSON.stringify(this.state.config)).then((response) => {
            if (response.status === 200) {
                this.props.goBack();
            } else if (response.status === 401) {
                console.log("Not authorized");
                reportInvalidToken();
            } else {
                this.setState({saveError: "Error setting config: " + response.status});
            }
        });
    }

    render() {
        let subTitleStyle = {
            width: 180,
        }

        let changesMade = this.areChangesPresent();
        if (!changesMade) {
            // eslint-disable-next-line react/no-direct-mutation-state
            this.state.saveError = null;
        }

        return (
            <div>
                <div style={{
                    display: "flex",
                    flexDirection: "row",
                    alignItems: "flex-end",
                }}>
                    <h1 style={{marginLeft: 40, paddingTop: 20}}>Settings</h1>
                    {!changesMade ? null : <button
                        className={"solid-button"}
                        onClick={this.saveChanges}
                        style={{marginLeft: 20, marginTop: 20}}>Save Changes</button>
                    }
                    <p style={{marginLeft: 20}} className={"error-text"}>{this.state.saveError}</p>
                </div>
                <span style={{
                    marginLeft: 40,
                    paddingTop: 10,
                    display: 'block',
                    cursor: 'pointer',
                    fontFamily: 'D-DIN-Bold',
                    fontSize: 20,
                    color: '#A0A0A0',
                }} onClick={this.props.goBack}>GO BACK</span>
                <div style={settingsBlockStyle}>
                    <div style={subTitleStyle}>
                        <h2 style={{borderBottom: '2px solid white', width: 'fit-content'}}>Scheduling</h2>
                    </div>
                    <div style={settingsSubBlockStyle}>
                        <SettingsTextBox
                            value={this.state.config != null ? this.state.config.pump_on_time : ""}
                            name={"Pump On Time"}
                            description={"The amount of time in minutes that the pump should be on for at a time."}
                            unit={"min"}
                            onChange={(value) => {
                                let config = this.state.config;
                                let errors = this.state.errors;
                                let vNum = parseFloat(value);
                                if (vNum < 1) {
                                    config.pump_on_time = value;
                                    errors.pump_on_time = "Value must be greater than or equal to 1.";
                                } else if (/^[+-]?([0-9]*[.])?[0-9]+$/.test(value)) {
                                    config.pump_on_time = vNum;
                                    delete errors.pump_on_time;
                                } else {
                                    config.pump_on_time = value;
                                    errors.pump_on_time = "Value must be a valid number.";
                                }
                                this.setState({config: config, errors: errors});
                            }}
                            error={this.state.errors.pump_on_time}
                        />
                        <SettingsTextBox
                            value={this.state.config != null ? this.state.config.pump_on_count : ""}
                            name={"Number of Pump On Times"}
                            description={"The count of the number of times the pump should be on per day."}
                            unit={"times per day"}
                            onChange={(value) => {
                                let config = this.state.config;
                                let errors = this.state.errors;
                                let vNum = parseFloat(value);
                                if (vNum < 1) {
                                    config.pump_on_count = value;
                                    errors.pump_on_count = "Value must be greater than or equal to 1.";
                                } else if (/^[+-]?([0-9]*[.])?[0-9]+$/.test(value)) {
                                    config.pump_on_count = vNum;
                                    delete errors.pump_on_count;
                                } else {
                                    config.pump_on_count = value;
                                    errors.pump_on_count = "Value must be a valid number.";
                                }
                                this.setState({config: config, errors: errors});
                            }}
                            error={this.state.errors.pump_on_count}
                        />
                        <SettingsTextBox
                            value={this.state.config != null ? this.state.config.ph_maximum_deviation : ""}
                            name={"pH Maximum Deviation"}
                            description={"The maximum amount of deviation from a plant's ideal pH range before preventing you from adding a plant to the system."}
                            unit={"pH"}
                            onChange={(value) => {
                                let config = this.state.config;
                                let errors = this.state.errors;
                                let vNum = parseFloat(value);
                                if (/^[+-]?([0-9]*[.])?[0-9]+$/.test(value)) {
                                    config.pump_on_count = vNum;
                                    delete errors.ph_maximum_deviation;
                                } else {
                                    config.ph_maximum_deviation = value;
                                    errors.ph_maximum_deviation = "Value must be a valid number.";
                                }
                                this.setState({config: config, errors: errors});
                            }}
                            error={this.state.errors.ph_maximum_deviation}
                        />
                        <SettingsTextBox
                            value={this.state.config != null ? this.state.config.ec_maximum_deviation : ""}
                            name={"EC Maximum Deviation"}
                            description={"The maximum amount of deviation from a plant's ideal EC range before preventing you from adding a plant to the system."}
                            unit={"mS/cm²"}
                            onChange={(value) => {
                                let config = this.state.config;
                                let errors = this.state.errors;
                                let vNum = parseFloat(value);
                                if (/^[+-]?([0-9]*[.])?[0-9]+$/.test(value)) {
                                    config.pump_on_count = vNum;
                                    delete errors.ec_maximum_deviation;
                                } else {
                                    config.ec_maximum_deviation = value;
                                    errors.ec_maximum_deviation = "Value must be a valid number.";
                                }
                                this.setState({config: config, errors: errors});
                            }}
                            error={this.state.errors.ec_maximum_deviation}
                        />
                        <SettingsTextBox
                            value={this.state.config != null ? this.state.config.light_maximum_deviation : ""}
                            name={"Light Maximum Deviation"}
                            description={"The maximum amount of deviation from a plant's ideal light range before preventing you from adding a plant to the system."}
                            unit={"hours"}
                            onChange={(value) => {
                                let config = this.state.config;
                                let errors = this.state.errors;
                                let vNum = parseFloat(value);
                                if (/^[+-]?([0-9]*[.])?[0-9]+$/.test(value)) {
                                    config.pump_on_count = vNum;
                                    delete errors.light_maximum_deviation;
                                } else {
                                    config.light_maximum_deviation = value;
                                    errors.light_maximum_deviation = "Value must be a valid number.";
                                }
                                this.setState({config: config, errors: errors});
                            }}
                            error={this.state.errors.light_maximum_deviation}
                        />
                    </div>
                </div>
                <div style={settingsBlockStyle}>
                    <div style={subTitleStyle}>
                        <h2 style={{borderBottom: '2px solid white', width: 'fit-content'}}>Alerting</h2>
                    </div>
                    <div style={settingsSubBlockStyle}>
                        <SettingsTextBox
                            value={this.state.config != null ? this.state.config.water_leak_amount : ""}
                            name={"Water Leak Amount"}
                            description={"The rate of water loss while the pump is on before an alert is triggered."}
                            unit={"ml/min"}
                            onChange={(value) => {
                                let config = this.state.config;
                                let errors = this.state.errors;
                                let vNum = parseFloat(value);
                                if (vNum < 1) {
                                    config.water_leak_amount = value;
                                    errors.water_leak_amount = "Value must be greater than or equal to 1.";
                                } else if (/^[+-]?([0-9]*[.])?[0-9]+$/.test(value)) {
                                    config.water_leak_amount = vNum;
                                    delete errors.water_leak_amount;
                                } else {
                                    config.water_leak_amount = value;
                                    errors.water_leak_amount = "Value must be a valid number.";
                                }
                                this.setState({config: config, errors: errors});
                            }}
                            error={this.state.errors.water_leak_amount}
                        />
                        <SettingsTextBox
                            value={this.state.config != null ? this.state.config.light_degredation_threshold : ""}
                            name={"Light Degradation Threshold"}
                            description={"The minimum amount of light that must be produced by the light while on to be considered healthy."}
                            unit={"uW/cm²"}
                            onChange={(value) => {
                                let config = this.state.config;
                                let errors = this.state.errors;
                                let vNum = parseFloat(value);
                                if (vNum < 1) {
                                    config.light_degredation_threshold = value;
                                    errors.light_degredation_threshold = "Value must be greater than or equal to 1.";
                                } else if (/^[+-]?([0-9]*[.])?[0-9]+$/.test(value)) {
                                    config.light_degredation_threshold = vNum;
                                    delete errors.light_degredation_threshold;
                                } else {
                                    config.light_degredation_threshold = value;
                                    errors.light_degredation_threshold = "Value must be a valid number.";
                                }
                                this.setState({config: config, errors: errors});
                            }}
                            error={this.state.errors.light_degredation_threshold}
                        />
                        <SettingsTextBox
                            value={this.state.config != null ? this.state.config.light_on_threshold : ""}
                            name={"Light On Threshold"}
                            description={"The minimum amount of light that must be produced by the light while on to be considered not failed."}
                            unit={"uW/cm²"}
                            onChange={(value) => {
                                let config = this.state.config;
                                let errors = this.state.errors;
                                let vNum = parseFloat(value);
                                if (vNum < 1) {
                                    config.light_on_threshold = value;
                                    errors.light_on_threshold = "Value must be greater than or equal to 1.";
                                } else if (/^[+-]?([0-9]*[.])?[0-9]+$/.test(value)) {
                                    config.light_on_threshold = vNum;
                                    delete errors.light_on_threshold;
                                } else {
                                    config.light_on_threshold = value;
                                    errors.light_on_threshold = "Value must be a valid number.";
                                }
                                this.setState({config: config, errors: errors});
                            }}
                            error={this.state.errors.light_on_threshold}
                        />
                        <SettingsTextBox
                            value={this.state.config != null ? this.state.config.minimum_water_level : ""}
                            name={"Minimum Water Level"}
                            description={"The minimum amount of water before an alert is triggered."}
                            unit={"in"}
                            onChange={(value) => {
                                let config = this.state.config;
                                let errors = this.state.errors;
                                let vNum = parseFloat(value);
                                if (vNum < 1) {
                                    config.minimum_water_level = value;
                                    errors.minimum_water_level = "Value must be greater than or equal to 1.";
                                } else if (/^[+-]?([0-9]*[.])?[0-9]+$/.test(value)) {
                                    config.minimum_water_level = vNum;
                                    delete errors.minimum_water_level;
                                } else {
                                    config.minimum_water_level = value;
                                    errors.minimum_water_level = "Value must be a valid number.";
                                }
                                this.setState({config: config, errors: errors});
                            }}
                            error={this.state.errors.minimum_water_level}
                        />
                        <SettingsTextBox
                            value={this.state.config != null ? this.state.config.minimum_co2_ppm : ""}
                            name={"Minimum CO2 Level"}
                            description={"The minimum amount of CO2 before an alert is triggered."}
                            unit={"ppm"}
                            onChange={(value) => {
                                let config = this.state.config;
                                let errors = this.state.errors;
                                let vNum = parseFloat(value);
                                if (vNum < 1) {
                                    config.minimum_co2_ppm = value;
                                    errors.minimum_co2_ppm = "Value must be greater than or equal to 1.";
                                } else if (/^[+-]?([0-9]*[.])?[0-9]+$/.test(value)) {
                                    config.minimum_co2_ppm = vNum;
                                    delete errors.minimum_co2_ppm;
                                } else {
                                    config.minimum_co2_ppm = value;
                                    errors.minimum_co2_ppm = "Value must be a valid number.";
                                }
                                this.setState({config: config, errors: errors});
                            }}
                            error={this.state.errors.minimum_co2_ppm}
                        />
                        <SettingsTextBox
                            value={this.state.config != null ? this.state.config.maximum_co2_ppm : ""}
                            name={"Maximum CO2 Level"}
                            description={"The maximum amount of CO2 before an alert is triggered."}
                            unit={"ppm"}
                            onChange={(value) => {
                                let config = this.state.config;
                                let errors = this.state.errors;
                                let vNum = parseFloat(value);
                                if (vNum < 1) {
                                    config.maximum_co2_ppm = value;
                                    errors.maximum_co2_ppm = "Value must be greater than or equal to 1.";
                                } else if (/^[+-]?([0-9]*[.])?[0-9]+$/.test(value)) {
                                    config.maximum_co2_ppm = vNum;
                                    delete errors.maximum_co2_ppm;
                                } else {
                                    config.maximum_co2_ppm = value;
                                    errors.maximum_co2_ppm = "Value must be a valid number.";
                                }
                                this.setState({config: config, errors: errors});
                            }}
                            error={this.state.errors.maximum_co2_ppm}
                        />
                        <SettingsTextBox
                            value={this.state.config != null ? this.state.config.minimum_air_temp : ""}
                            name={"Minimum Air Temperature"}
                            description={"The minimum air temperature before an alert is triggered."}
                            unit={"°F"}
                            onChange={(value) => {
                                let config = this.state.config;
                                let errors = this.state.errors;
                                let vNum = parseFloat(value);
                                if (vNum < 1) {
                                    config.minimum_air_temp = value;
                                    errors.minimum_air_temp = "Value must be greater than or equal to 1.";
                                } else if (/^[+-]?([0-9]*[.])?[0-9]+$/.test(value)) {
                                    config.minimum_air_temp = vNum;
                                    delete errors.minimum_air_temp;
                                } else {
                                    config.minimum_air_temp = value;
                                    errors.minimum_air_temp = "Value must be a valid number.";
                                }
                                this.setState({config: config, errors: errors});
                            }}
                            error={this.state.errors.minimum_air_temp}
                        />
                        <SettingsTextBox
                            value={this.state.config != null ? this.state.config.maximum_air_temp : ""}
                            name={"Maximum Air Temperature"}
                            description={"The maximum air temperature before an alert is triggered."}
                            unit={"°F"}
                            onChange={(value) => {
                                let config = this.state.config;
                                let errors = this.state.errors;
                                let vNum = parseFloat(value);
                                if (vNum < 1) {
                                    config.maximum_air_temp = value;
                                    errors.maximum_air_temp = "Value must be greater than or equal to 1.";
                                } else if (/^[+-]?([0-9]*[.])?[0-9]+$/.test(value)) {
                                    config.maximum_air_temp = vNum;
                                    delete errors.maximum_air_temp;
                                } else {
                                    config.maximum_air_temp = value;
                                    errors.maximum_air_temp = "Value must be a valid number.";
                                }
                                this.setState({config: config, errors: errors});
                            }}
                            error={this.state.errors.maximum_air_temp}
                        />
                        <SettingsTextBox
                            value={this.state.config != null ? this.state.config.minimum_air_humidity : ""}
                            name={"Minimum Air Humidity"}
                            description={"The minimum air relative humidity before an alert is triggered."}
                            unit={"% RH"}
                            onChange={(value) => {
                                let config = this.state.config;
                                let errors = this.state.errors;
                                let vNum = parseFloat(value);
                                if (vNum < 1) {
                                    config.minimum_air_humidity = value;
                                    errors.minimum_air_humidity = "Value must be greater than or equal to 1.";
                                } else if (/^[+-]?([0-9]*[.])?[0-9]+$/.test(value)) {
                                    config.minimum_air_humidity = vNum;
                                    delete errors.minimum_air_humidity;
                                } else {
                                    config.minimum_air_humidity = value;
                                    errors.minimum_air_humidity = "Value must be a valid number.";
                                }
                                this.setState({config: config, errors: errors});
                            }}
                            error={this.state.errors.minimum_air_humidity}
                        />
                        <SettingsTextBox
                            value={this.state.config != null ? this.state.config.maximum_air_humidity : ""}
                            name={"Maximum Air Humidity"}
                            description={"The maximum air relative humidity before an alert is triggered."}
                            unit={"% RH"}
                            onChange={(value) => {
                                let config = this.state.config;
                                let errors = this.state.errors;
                                let vNum = parseFloat(value);
                                if (vNum < 1) {
                                    config.maximum_air_humidity = value;
                                    errors.maximum_air_humidity = "Value must be greater than or equal to 1.";
                                } else if (/^[+-]?([0-9]*[.])?[0-9]+$/.test(value)) {
                                    config.maximum_air_humidity = vNum;
                                    delete errors.maximum_air_humidity;
                                } else {
                                    config.maximum_air_humidity = value;
                                    errors.maximum_air_humidity = "Value must be a valid number.";
                                }
                                this.setState({config: config, errors: errors});
                            }}
                            error={this.state.errors.maximum_air_humidity}
                        />
                    </div>
                </div>
                <div style={settingsBlockStyle}>
                    <div style={subTitleStyle}>
                        <h2 style={{borderBottom: '2px solid white', width: 'fit-content'}}>Constants</h2>
                    </div>
                    <div style={settingsSubBlockStyle}>
                        <SettingsTextBox
                            value={this.state.config != null ? this.state.config.ec_per_vol_nutrient : ""}
                            name={"Delta EC per Volume of Nutrient"}
                            description={"For one mL of nutrient, and one liter of water, the change in electrical conductivity (EC)."}
                            unit={"mS/cm²/ml/L"}
                            onChange={(value) => {
                                let config = this.state.config;
                                let errors = this.state.errors;
                                let vNum = parseFloat(value);
                                if (/^[+-]?([0-9]*[.])?[0-9]+$/.test(value)) {
                                    config.ec_per_vol_nutrient = vNum;
                                    delete errors.ec_per_vol_nutrient;
                                } else {
                                    config.ec_per_vol_nutrient = value;
                                    errors.ec_per_vol_nutrient = "Value must be a valid number.";
                                }
                                this.setState({config: config, errors: errors});
                            }}
                            error={this.state.errors.ec_per_vol_nutrient}
                        />
                        <SettingsTextBox
                            value={this.state.config != null ? this.state.config.ph_up_per_vol_solution : ""}
                            name={"Delta pH per Volume of pH Up"}
                            description={"For one mL of pH Up, and one liter of water, the amount pH increases by."}
                            unit={"pH/ml/L"}
                            onChange={(value) => {
                                let config = this.state.config;
                                let errors = this.state.errors;
                                let vNum = parseFloat(value);
                                if (/^[+-]?([0-9]*[.])?[0-9]+$/.test(value)) {
                                    config.ph_up_per_vol_solution = vNum;
                                    delete errors.ph_up_per_vol_solution;
                                } else {
                                    config.ph_up_per_vol_solution = value;
                                    errors.ph_up_per_vol_solution = "Value must be a valid number.";
                                }
                                this.setState({config: config, errors: errors});
                            }}
                            error={this.state.errors.ph_up_per_vol_solution}
                        />
                        <SettingsTextBox
                            value={this.state.config != null ? this.state.config.ph_down_per_vol_solution : ""}
                            name={"Delta pH per Volume of pH Down"}
                            description={"For one mL of pH Down, and one liter of water, the amount pH decreases by."}
                            unit={"pH/ml/L"}
                            onChange={(value) => {
                                let config = this.state.config;
                                let errors = this.state.errors;
                                let vNum = parseFloat(value);
                                if (/^[+-]?([0-9]*[.])?[0-9]+$/.test(value)) {
                                    config.ph_down_per_vol_solution = vNum;
                                    delete errors.ph_down_per_vol_solution;
                                } else {
                                    config.ph_down_per_vol_solution = value;
                                    errors.ph_down_per_vol_solution = "Value must be a valid number.";
                                }
                                this.setState({config: config, errors: errors});
                            }}
                            error={this.state.errors.ph_down_per_vol_solution}
                        />
                    </div>
                </div>
                <div style={settingsBlockStyle}>
                    <div style={subTitleStyle}>
                        <h2 style={{borderBottom: '2px solid white', width: 'fit-content'}}>Physical Specifications</h2>
                    </div>
                    <div style={settingsSubBlockStyle}>
                        <SettingsTextBox
                            value={this.state.config != null ? this.state.config.tank_total_volume : ""}
                            name={"Tank Total Volume"}
                            description={"The volume of the tank in liters up to the max fill line."}
                            unit={"L"}
                            onChange={(value) => {
                                let config = this.state.config;
                                let errors = this.state.errors;
                                let vNum = parseFloat(value);
                                if (vNum < 1) {
                                    config.tank_total_volume = value;
                                    errors.tank_total_volume = "Value must be greater than or equal to 1.";
                                } else if (/^[+-]?([0-9]*[.])?[0-9]+$/.test(value)) {
                                    config.tank_total_volume = vNum;
                                    delete errors.tank_total_volume;
                                } else {
                                    config.tank_total_volume = value;
                                    errors.tank_total_volume = "Value must be a valid number.";
                                }
                                this.setState({config: config, errors: errors});
                            }}
                            error={this.state.errors.tank_total_volume}
                        />
                        <SettingsTextBox
                            value={this.state.config != null ? this.state.config.tank_usable_volume : ""}
                            name={"Tank Usable Volume"}
                            description={"The volume of the tank in liters between the fill line and the minimum point (2 in)."}
                            unit={"L"}
                            onChange={(value) => {
                                let config = this.state.config;
                                let errors = this.state.errors;
                                let vNum = parseFloat(value);
                                if (vNum < 1) {
                                    config.tank_usable_volume = value;
                                    errors.tank_usable_volume = "Value must be greater than or equal to 1.";
                                } else if (/^[+-]?([0-9]*[.])?[0-9]+$/.test(value)) {
                                    config.tank_usable_volume = vNum;
                                    delete errors.tank_usable_volume;
                                } else {
                                    config.tank_usable_volume = value;
                                    errors.tank_usable_volume = "Value must be a valid number.";
                                }
                                this.setState({config: config, errors: errors});
                            }}
                            error={this.state.errors.tank_usable_volume}
                        />
                        <SettingsTextBox
                            value={this.state.config != null ? this.state.config.light_wattage : ""}
                            name={"Light Power Consumption"}
                            description={"The amount of power the light draws when on."}
                            unit={"watts"}
                            onChange={(value) => {
                                let config = this.state.config;
                                let errors = this.state.errors;
                                let vNum = parseFloat(value);
                                if (vNum < 1) {
                                    config.light_wattage = value;
                                    errors.light_wattage = "Value must be greater than or equal to 1.";
                                } else if (/^[+-]?([0-9]*[.])?[0-9]+$/.test(value)) {
                                    config.light_wattage = vNum;
                                    delete errors.light_wattage;
                                } else {
                                    config.light_wattage = value;
                                    errors.light_wattage = "Value must be a valid number.";
                                }
                                this.setState({config: config, errors: errors});
                            }}
                            error={this.state.errors.light_wattage}
                        />
                        <SettingsTextBox
                            value={this.state.config != null ? this.state.config.pump_wattage : ""}
                            name={"Pump Power Consumption"}
                            description={"The amount of power the pump draws when on."}
                            unit={"watts"}
                            onChange={(value) => {
                                let config = this.state.config;
                                let errors = this.state.errors;
                                let vNum = parseFloat(value);
                                if (vNum < 1) {
                                    config.pump_wattage = value;
                                    errors.pump_wattage = "Value must be greater than or equal to 1.";
                                } else if (/^[+-]?([0-9]*[.])?[0-9]+$/.test(value)) {
                                    config.pump_wattage = vNum;
                                    delete errors.pump_wattage;
                                } else {
                                    config.pump_wattage = value;
                                    errors.pump_wattage = "Value must be a valid number.";
                                }
                                this.setState({config: config, errors: errors});
                            }}
                            error={this.state.errors.pump_wattage}
                        />
                    </div>
                </div>
                <div style={settingsBlockStyle}>
                    <div style={subTitleStyle}>
                        <h2 style={{borderBottom: '2px solid white', width: 'fit-content'}}>Plant Configurations</h2>
                    </div>
                    <div style={settingsSubBlockStyle}>
                        {this.state.config == null ? null : this.state.config.plant_configs.map((plantConfig, index) => {
                            return (
                                <PlantConfigurationSettings
                                    plant={plantConfig}
                                    errors={this.state.errors.plant_configs[index]}
                                    onChange={() => {this.setState({})}}
                                    onDelete={() => {
                                        this.deletePlantConfig(index);
                                    }}
                                />
                            );
                        })}
                        <button className={"solid-button"} onClick={this.addPlantConfig}>ADD PLANT CONFIGURATION</button>
                    </div>
                </div>
                <div style={{height: 50}}/>
            </div>
        )
    }
}