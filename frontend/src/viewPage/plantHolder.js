import React from 'react';
import {PlantHolderInfo} from "./plantHolderInfo";

export class PlantHolder extends React.Component {
    constructor(props) {
        super(props);

        this.getSelectedPlantConfig = this.getSelectedPlantConfig.bind(this);
        this.onMouseOver = this.onMouseOver.bind(this);
        this.onMouseLeave = this.onMouseLeave.bind(this);

        this.state = {hover: false};
    }

    getSelectedPlantConfig(plantId) {
        for (let p of this.props.plantConfigs) {
            if (p.plant_id === plantId) {
                return p;
            }
        }

        return {color1: "#858585", plant_id: -1};
    }

    onMouseOver() {
        if (this.props.selected) {
            return;
        }

        this.props.setHovered(true);
        this.setState({hover: true});
    }

    onMouseLeave() {
        if (this.props.selected) {
            return;
        }

        this.props.setHovered(false);
        this.setState({hover: false});
    }

    render() {
        let config = this.getSelectedPlantConfig(this.props.plant.plant_id);
        let showHidden = this.props.selected || this.state.hover;

        return (
            <div style={{
                position: 'absolute',
                top: (17.88 + this.props.plant.y_coordinate * 16.88).toString() + 'vh',
                left: 'calc(' + (18.47 + this.props.plant.x_coordinate * 16.88).toString() + 'vh - 150px)',
            }}>
                <div style={{
                        backgroundColor: config.color1,
                        height: '12vh',
                        width: '12vh',
                        borderRadius: '6vh',
                        opacity: showHidden ? 0.6 : 0.45,
                        cursor: 'pointer',
                    }}
                     onMouseOver={this.onMouseOver}
                     onMouseLeave={this.onMouseLeave}
                     onClick={() => {
                         if (this.props.selected) {
                             this.props.setSelected(null);
                         } else {
                             this.props.setSelected(this.props.plant);
                         }
                     }}
                />

                <div style={{
                    height: 'calc(14vh - 6px)',
                    width: 'calc(14vh - 6px)',
                    borderRadius: '7vh',
                    position: 'absolute',
                    border: '3px solid white',
                    top: '-1vh',
                    left: '-1vh',
                    pointerEvents: 'none',
                    opacity: showHidden ? 1 : 0,
                }}/>
                {/* The diagonal line leading away from the circle */}
                {!showHidden ? null : <div style={{
                    transform: 'rotate(45deg)',
                    backgroundColor: 'white',
                    height: 3,
                    width: '4vh',
                    top: '10.879vh',
                    left: '10.879vh',
                    position: 'absolute',
                    transformOrigin: '0 0',
                }}/>}
                {!showHidden ? null : <div style={{
                    backgroundColor: 'white',
                    height: 3,
                    width: ((4-this.props.plant.x_coordinate) * 16.88).toString() + 'vh',
                    top: '13.65vh',
                    left: '13.5vh',
                    position: 'absolute',
                }}/>}
                {!showHidden ? null : <PlantHolderInfo
                    plantConfig={config}
                    configs={this.props.plantConfigs}
                    plant={this.props.plant}
                    redrawOverlay={this.props.redrawOverlay}
                    getConfig={this.getSelectedPlantConfig}
                    style={{
                        position: 'absolute',
                        top: this.props.plant.y_coordinate >= 2 ? 'calc(13.65vh - 221px)' : '13.65vh',
                        left: (13.5 + (4-this.props.plant.x_coordinate) * 16.88).toString() + 'vh',
                        height: 200,
                        width: 'calc(14vh - 6px)',
                }}/>}

            </div>
        );
    }
}