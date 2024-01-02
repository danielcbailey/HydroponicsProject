import React from 'react';
import {SettingsTextBox} from "./settingsTextBox";

let containerStyle = {
    display: 'flex',
    flexDirection: 'column',
    alignItems: 'flex-start',
    gap: 10,
    paddingLeft: 50,
    paddingTop: 10,
    paddingBottom: 10,
    paddingRight: 40,
    borderLeft: '50px solid white',
    borderTop: '2px solid white',
    borderBottom: '2px solid white',
    borderRight: '2px solid white',
    position: 'relative',
};

let labelStyle = {
    backgroundColor: 'white',
    color: 'black',
    fontFamily: 'D-DIN-Bold',
    transformOrigin: '0 0',
    transform: 'rotate(90deg)',
    position: 'absolute',
    whiteSpace: 'nowrap',
    fontSize: 24,
    padding: 10,
    left: 0,
    top: 0,
}

export class PlantConfigurationSettings extends React.Component {
    render() {
        return (
            <div style={containerStyle}>
                <div style={labelStyle}>
                    {this.props.plant.name}
                </div>

                <SettingsTextBox
                    value={this.props.plant.name}
                    name={"Plant Type Name"}
                    description={"Plant configurations are prototypical configurations for a plant. You can think of it as a plant type."}
                    unit={""}
                    onChange={(value) => {
                        let plant = this.props.plant;
                        let errors = this.props.errors;

                        plant.name = value;
                        if (value === "") {
                            errors.name = "A name is required.";
                        } else {
                            delete errors.name;
                        }

                        this.props.onChange(plant, errors);
                    }}
                    error={this.props.errors.name}
                />
                <SettingsTextBox
                    value={this.props.plant.transplant_to_harvest_days}
                    name={"Days Until Harvest"}
                    description={"The number of days between germination and the start of harvest."}
                    unit={"days"}
                    onChange={(value) => {
                        let plant = this.props.plant;
                        let errors = this.props.errors;

                        let vNum = parseFloat(value);
                        if (vNum < 1) {
                            plant.transplant_to_harvest_days = value;
                            errors.transplant_to_harvest_days = "Value must be greater than or equal to 1.";
                        } else if (/^[+-]?([0-9]*[.])?[0-9]+$/.test(value)) {
                            plant.transplant_to_harvest_days = vNum;
                            delete errors.transplant_to_harvest_days;
                        } else {
                            plant.transplant_to_harvest_days = value;
                            errors.transplant_to_harvest_days = "Value must be a valid number.";
                        }

                        this.props.onChange(plant, errors);
                    }}
                    error={this.props.errors.transplant_to_harvest_days}
                />
                <SettingsTextBox
                    value={this.props.plant.harvest_length_days}
                    name={"Harvest Length"}
                    description={"The number of days between the start of the harvest period and the end."}
                    unit={"days"}
                    onChange={(value) => {
                        let plant = this.props.plant;
                        let errors = this.props.errors;

                        let vNum = parseFloat(value);
                        if (vNum < 0) {
                            plant.harvest_length_days = value;
                            errors.harvest_length_days = "Value must be greater than or equal to 0.";
                        } else if (/^[+-]?([0-9]*[.])?[0-9]+$/.test(value)) {
                            plant.harvest_length_days = vNum;
                            delete errors.harvest_length_days;
                        } else {
                            plant.harvest_length_days = value;
                            errors.harvest_length_days = "Value must be a valid number.";
                        }

                        this.props.onChange(plant, errors);
                    }}
                    error={this.props.errors.harvest_length_days}
                />
                <SettingsTextBox
                    value={this.props.plant.ec_lower_limit}
                    name={"Minimum EC"}
                    description={"The minimum recommended EC nutrient concentration for this plant."}
                    unit={"mS/cm²"}
                    onChange={(value) => {
                        let plant = this.props.plant;
                        let errors = this.props.errors;

                        let vNum = parseFloat(value);
                        if (vNum < 0.25) {
                            plant.ec_lower_limit = value;
                            errors.ec_lower_limit = "Value must be greater than or equal to 0.25.";
                        } else if (/^[+-]?([0-9]*[.])?[0-9]+$/.test(value)) {
                            plant.ec_lower_limit = vNum;
                            delete errors.ec_lower_limit;
                        } else {
                            plant.ec_lower_limit = value;
                            errors.ec_lower_limit = "Value must be a valid number.";
                        }

                        this.props.onChange(plant, errors);
                    }}
                    error={this.props.errors.ec_lower_limit}
                />
                <SettingsTextBox
                    value={this.props.plant.ec_upper_limit}
                    name={"Maximum EC"}
                    description={"The maximum recommended EC nutrient concentration for this plant."}
                    unit={"mS/cm²"}
                    onChange={(value) => {
                        let plant = this.props.plant;
                        let errors = this.props.errors;

                        let vNum = parseFloat(value);
                        if (vNum < 1) {
                            plant.ec_upper_limit = value;
                            errors.ec_upper_limit = "Value must be greater than or equal to 1.";
                        } else if (/^[+-]?([0-9]*[.])?[0-9]+$/.test(value)) {
                            plant.ec_upper_limit = vNum;
                            delete errors.ec_upper_limit;
                        } else {
                            plant.ec_upper_limit = value;
                            errors.ec_upper_limit = "Value must be a valid number.";
                        }

                        this.props.onChange(plant, errors);
                    }}
                    error={this.props.errors.ec_upper_limit}
                />
                <SettingsTextBox
                    value={this.props.plant.ph_lower_limit}
                    name={"Minimum pH"}
                    description={"The minimum recommended pH level for this plant."}
                    unit={"pH"}
                    onChange={(value) => {
                        let plant = this.props.plant;
                        let errors = this.props.errors;

                        let vNum = parseFloat(value);
                        if (vNum < 0) {
                            plant.ph_lower_limit = value;
                            errors.ph_lower_limit = "Value must be greater than or equal to 0.";
                        } else if (/^[+-]?([0-9]*[.])?[0-9]+$/.test(value)) {
                            plant.ph_lower_limit = vNum;
                            delete errors.ph_lower_limit;
                        } else {
                            plant.ph_lower_limit = value;
                            errors.ph_lower_limit = "Value must be a valid number.";
                        }

                        this.props.onChange(plant, errors);
                    }}
                    error={this.props.errors.ph_lower_limit}
                />
                <SettingsTextBox
                    value={this.props.plant.ph_upper_limit}
                    name={"Maximum pH"}
                    description={"The maximum recommended pH level for this plant."}
                    unit={"pH"}
                    onChange={(value) => {
                        let plant = this.props.plant;
                        let errors = this.props.errors;

                        let vNum = parseFloat(value);
                        if (vNum < 0) {
                            plant.ph_upper_limit = value;
                            errors.ph_upper_limit = "Value must be greater than or equal to 0.";
                        } else if (/^[+-]?([0-9]*[.])?[0-9]+$/.test(value)) {
                            plant.ph_upper_limit = vNum;
                            delete errors.ph_upper_limit;
                        } else {
                            plant.ph_upper_limit = value;
                            errors.ph_upper_limit = "Value must be a valid number.";
                        }

                        this.props.onChange(plant, errors);
                    }}
                    error={this.props.errors.ph_upper_limit}
                />
                <SettingsTextBox
                    value={this.props.plant.light_hours_lower_limit}
                    name={"Minimum Light Time"}
                    description={"The minimum recommended light time for this plant."}
                    unit={"hours"}
                    onChange={(value) => {
                        let plant = this.props.plant;
                        let errors = this.props.errors;

                        let vNum = parseFloat(value);
                        if (vNum < 0) {
                            plant.light_hours_lower_limit = value;
                            errors.light_hours_lower_limit = "Value must be greater than or equal to 0.";
                        } else if (/^[+-]?([0-9]*[.])?[0-9]+$/.test(value)) {
                            plant.light_hours_lower_limit = vNum;
                            delete errors.light_hours_lower_limit;
                        } else {
                            plant.light_hours_lower_limit = value;
                            errors.light_hours_lower_limit = "Value must be a valid number.";
                        }

                        this.props.onChange(plant, errors);
                    }}
                    error={this.props.errors.light_hours_lower_limit}
                />
                <SettingsTextBox
                    value={this.props.plant.light_hours_upper_limit}
                    name={"Maximum Light Time"}
                    description={"The maximum recommended light time for this plant."}
                    unit={"hours"}
                    onChange={(value) => {
                        let plant = this.props.plant;
                        let errors = this.props.errors;

                        let vNum = parseFloat(value);
                        if (vNum < 0) {
                            plant.light_hours_upper_limit = value;
                            errors.light_hours_upper_limit = "Value must be greater than or equal to 0.";
                        } else if (/^[+-]?([0-9]*[.])?[0-9]+$/.test(value)) {
                            plant.light_hours_upper_limit = vNum;
                            delete errors.light_hours_upper_limit;
                        } else {
                            plant.light_hours_upper_limit = value;
                            errors.light_hours_upper_limit = "Value must be a valid number.";
                        }

                        this.props.onChange(plant, errors);
                    }}
                    error={this.props.errors.light_hours_upper_limit}
                />
                <SettingsTextBox
                    value={this.props.plant.color1}
                    name={"Color"}
                    description={"The display hex color code for this plant."}
                    unit={""}
                    onChange={(value) => {
                        let plant = this.props.plant;
                        let errors = this.props.errors;

                        if (/^#[0-9a-fA-F]{6}$/.test(value)) {
                            plant.color1 = value;
                            delete errors.color1;
                        } else {
                            plant.color1 = value;
                            errors.color1 = "Value must be a 6 digit hex color code.";
                        }

                        this.props.onChange(plant, errors);
                    }}
                    error={this.props.errors.color1}
                />
                <button className={"solid-button"} onClick={this.props.onDelete}>DELETE</button>
                {this.props.errors.delete ? <p className={"error-text"}>{this.props.errors.delete}</p> : null}
            </div>
        )
    }
}
