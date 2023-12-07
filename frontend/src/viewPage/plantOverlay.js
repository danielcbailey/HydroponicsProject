import React from 'react';
import {PlantHolder} from "./plantHolder";

let legendStyle = {
    position: 'absolute',
    top: 50,
    right: 50,
    display: 'flex',
    flexDirection: 'column',
    alignItems: 'flex-start',
    gap: 10,
    width: 300,
};

export class PlantOverlay extends React.Component {
    constructor(props) {
        super(props);

        this.state = {
            selectedPlant: null,
            otherHovered: false,
        }
    }

    render() {
        let plantsConfigsInMachine = this.props.plantConfigs.filter((plant) => {
            for (let p of this.props.plantHolders) {
                if (p.plant_id === plant.plant_id) {
                    return true;
                }
            }
        });

        return (
            <div style={{width: 'calc(100% - 150px)', height: '100%', position: 'relative', left: 150}}>
                <div style={legendStyle}>
                    <h1 style={{borderBottom: '2px solid white', width: '100%'}}>Legend</h1>
                    {plantsConfigsInMachine.map((plantConfig) => {
                        return (
                            <div>
                                <div style={{
                                    width: 40,
                                    height: 40,
                                    backgroundColor: plantConfig.color1,
                                    opacity: 0.5,
                                    borderRadius: 20,
                                    display: 'inline-block',
                                }}/>
                                <span className={"body-text"} style={{
                                    position: 'relative',
                                    top: -14,
                                    paddingLeft: 20,
                                    display: 'inline-block',
                                }}>{plantConfig.name}</span>
                            </div>
                        )
                    })}
                </div>
                <div style={{
                    position: 'absolute',
                    top: '2.5vh',
                    left: 150,
                }}>
                    {this.props.plantHolders.map((plant) => {
                        return (
                            <PlantHolder
                                plant={plant}
                                plantConfigs={this.props.plantConfigs}
                                selected={this.state.selectedPlant === plant && !this.state.otherHovered}
                                setSelected={(selected) => {
                                    this.setState({selectedPlant: selected});
                                }}
                                setHovered={(hovered) => {
                                    this.setState({otherHovered: hovered});
                                }}
                                redrawOverlay={() => this.setState({})}
                            />
                        )
                    })}
                </div>
            </div>
        );
    }
}