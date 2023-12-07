import React from 'react';
import {MetricBlock} from "./metricBlock";
import {getSimpleDuration, httpPost, reportInvalidToken} from "../util";

let containerStyle = {
    display: 'flex',
    flexDirection: 'column',
    alignItems: 'flex-start',
    gap: 10,
    paddingLeft: 20,
    paddingTop: 10,
    paddingBottom: 10,
    paddingRight: 40,
    borderLeft: '50px solid white',
    borderTop: '2px solid white',
    borderBottom: '2px solid white',
    borderRight: '2px solid white',
    position: 'relative',
    height: '100%',
    width: 330,
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

export class PlantHolderInfo extends React.Component {
    constructor(props) {
        super(props);

        this.buttonClicked = this.buttonClicked.bind(this);

        this.state = {modifying: false, modifyingPlant: null};
    }

    buttonClicked() {
        if (!this.state.modifying) {
            let modifyingPlant = JSON.parse(JSON.stringify(this.props.plant));
            if (modifyingPlant.time_transplanted === 0) {
                modifyingPlant.time_transplanted = Math.round(Date.now() / 1000);
            }
            this.setState({modifying: true, modifyingPlant: modifyingPlant});
        } else {
            // must update the server
            let updatedHolder = this.state.modifyingPlant;
            if (this.state.modifyingPlant.plant_id === -1) {
                updatedHolder.time_transplanted = 0;
            }
            httpPost('/api/updatePlantHolder', JSON.stringify(updatedHolder)).then((response) => {
                if (response.status === 200) {
                    // success
                    this.setState({modifying: false});
                    this.props.plant.time_transplanted = updatedHolder.time_transplanted;
                    this.props.plant.plant_id = updatedHolder.plant_id;
                    this.props.redrawOverlay();
                } else if (response.status === 401) {
                    // unauthorized
                    reportInvalidToken();
                }
            });
        }
    }

    render() {
        let sincePlantingInfo = null;
        let untilHarvestingInfo = null;
        let untilRemovalInfo = null;

        if (this.props.plantConfig.name != null) {
            sincePlantingInfo = getSimpleDuration(Date.now() - this.props.plant.time_transplanted * 1000);
            untilHarvestingInfo = getSimpleDuration((this.props.plantConfig.transplant_to_harvest_days * 24 * 3600 + this.props.plant.time_transplanted) * 1000 - Date.now());
            untilRemovalInfo = getSimpleDuration(((this.props.plantConfig.transplant_to_harvest_days + this.props.plantConfig.harvest_length_days) * 24 * 3600 + this.props.plant.time_transplanted) * 1000 - Date.now());
        }

        let content = null;
        if (!this.state.modifying) {
            content = (<div style={{
                display: 'flex',
                flexDirection: 'row',
                alignItems: 'flex-start',
                gap: 30,
                marginTop: 0,
                paddingLeft: 30,
                marginBottom: 10,
                position: 'relative',
                left: -30,
            }}>
                {this.props.plant.time_transplanted === 0 ? null :
                    <MetricBlock
                        name={"Since Planting"}
                        value={sincePlantingInfo.value}
                        unit={sincePlantingInfo.unit}/>}
                {this.props.plant.time_transplanted === 0 ? null :
                    <MetricBlock
                        name={"Until Harvesting"}
                        value={untilHarvestingInfo.value}
                        unit={untilHarvestingInfo.unit}/>}
                {this.props.plant.time_transplanted === 0 ? null :
                    <MetricBlock
                        name={"Until Removal"}
                        value={untilRemovalInfo.value}
                        unit={untilRemovalInfo.unit}/>}
            </div>);
        } else {
            let modifyingPlantType = this.props.getConfig(this.state.modifyingPlant.plant_id);

            content = [
                <div>
                    <input
                        type={"text"}
                        className={"text-input"}
                        style={{width: 200}}
                        value={isNaN(this.state.modifyingPlant.time_transplanted) ? "" : Math.round((Date.now() - this.state.modifyingPlant.time_transplanted * 1000) / (24 * 3600 * 1000))}
                        onChange={(e) => {
                            let modifyingPlant = this.state.modifyingPlant;
                            modifyingPlant.time_transplanted = Math.round((Date.now() - (parseInt(e.target.value) * 24 * 3600 * 1000)) / 1000);
                            this.setState({modifyingPlant: modifyingPlant});
                        }}/>
                    <span className={"body-text"} style={{color: '#A0A0A0', marginLeft: 10}}>Days Old</span>
                </div>,
                <div>
                    <select
                        className={"text-input"}
                        style={{width: 200}}
                        value={modifyingPlantType.plant_id.toString()}
                        onChange={(e) => {
                            this.setState({modifyingPlant: {...this.state.modifyingPlant, plant_id: parseInt(e.target.value)}});
                        }}>
                        <option value={"-1"}>Empty</option>
                        {this.props.configs.map((plantConfig) => {
                            return <option value={plantConfig.plant_id.toString()}>{plantConfig.name}</option>
                        })}
                    </select>
                    <span className={"body-text"} style={{color: '#A0A0A0', marginLeft: 10}}>Plant Type</span>
                </div>
            ]
        }

        return (
            <div style={this.props.style}>
                <div style={containerStyle}>
                    <div style={labelStyle}>
                        {this.props.plantConfig.name != null ? this.props.plantConfig.name : "Empty"}
                    </div>
                    {content}
                    <button className={"solid-button"} onClick={this.buttonClicked} style={{
                        position: 'absolute',
                        bottom: 20,
                        left: 20
                    }}>{this.state.modifying ? "SAVE": "MODIFY PLANT"}</button>
                </div>
            </div>
        );
    }
}