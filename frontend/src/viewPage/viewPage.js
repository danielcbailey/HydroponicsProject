import React from 'react';
import "../App.css";
import "./viewPage.css";
import {VirtualFarm} from "../login/virtualFarm";
import {OverviewStats} from "./overviewStats";
import {PlantOverlay} from "./plantOverlay";
import {httpGet, reportInvalidToken} from "../util";
import {WaterOverlay} from "./waterOverlay";

export class ViewPage extends React.Component {
    constructor(props) {
        super(props);

        this.setNowShowing = this.setNowShowing.bind(this);

        this.state = {
            mode: "overview", // others are: "water", "plants"
            showing: "overview", // set by the transition animation
            config: null,
        }
    }

    componentDidMount() {
        httpGet("/api/getConfig").then((response) => {
            if (response.status === 200) {
                response.json().then((config) => {
                    this.setState({config: config});
                });
            } else if (response.status === 401) {
                // unauthorized
                reportInvalidToken();
            }
        });
    }

    setNowShowing(view) {
        this.setState({showing: view});
    }

    render() {
        let overlay = null;
        if (this.state.showing === "overview") {
            overlay = <OverviewStats/>
        } else if (this.state.showing === "plants" && this.state.config != null) {
            overlay = <PlantOverlay plantConfigs={this.state.config.plant_configs} plantHolders={this.state.config.plant_holders}/>
        } else if (this.state.showing === "water") {
            overlay = <WaterOverlay/>
        }

        return (
            <div style={{width: '100%', height: '100%'}}>
                <div style={{
                    paddingLeft: 15,
                    paddingTop: 10,
                    position: "absolute",
                    top: 0,
                    left: 0,
                }}>
                    {/*the navigation menu*/}
                    <span
                        className={this.state.mode === "overview" ? "navButton selectedNavButton" : "navButton"}
                        onClick={() => this.setState({mode: "overview"})}>HOME</span>
                    <span
                        className={this.state.mode === "plants" ? "navButton selectedNavButton" : "navButton"}
                        onClick={() => this.setState({mode: "plants"})}>PLANTS</span>
                    <span
                        className={this.state.mode === "water" ? "navButton selectedNavButton" : "navButton"}
                        onClick={() => this.setState({mode: "water"})}>WATER</span>
                    <span
                        className={"navButton"} onClick={this.props.switchToSettings}>SETTINGS</span>
                </div>
                <VirtualFarm view={this.state.mode} onNewView={this.setNowShowing} style={{
                    height: '95vh',
                    width: '95vh',
                    position: 'absolute',
                    top: '2.5vh',
                    left: '150px'
                }}/>
                {overlay}
            </div>
        );
    }
}