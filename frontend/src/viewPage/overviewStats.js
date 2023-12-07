import React from 'react';
import {addMetricsListener, getMetrics, removeMetricsListener} from "../metrics";
import {MetricBlock} from "./metricBlock";
import {httpPost, reportInvalidToken} from "../util";

let metricsCollectionStyle = {
    display: 'flex',
    flexDirection: 'row',
    alignItems: 'flex-start',
    justifyContent: 'space-between',
    gap: 30,
    borderTop: '2px solid white',
    marginTop: 10,
    marginBottom: 30,
}

export class OverviewStats extends React.Component {
    constructor(props) {
        super(props);

        this.componentDidMount = this.componentDidMount.bind(this);
        this.componentWillUnmount = this.componentWillUnmount.bind(this);
        this.metricsUpdate = this.metricsUpdate.bind(this);

        let metrics = getMetrics();
        if (metrics === null) {
            metrics = {
                air_temperature: 0,
                air_relative_humidity: 0,
                air_co2_ppm: 0,
                pH: 0,
                water_level: 0,
                electrical_conductivity: 0,
            }
        }

        this.state = {metrics: metrics};
    }

    componentDidMount() {
        addMetricsListener(this.metricsUpdate);
    }

    componentWillUnmount() {
        // remove metrics listener
        removeMetricsListener(this.metricsUpdate);
    }

    metricsUpdate(metrics) {
        this.setState({metrics: metrics});
    }

    render() {
        return (
            <div style={{
                position: 'absolute',
                right: 50,
                top: 0,
                marginTop: 50
            }}>
                <h1>Atmosphere</h1>
                <div style={metricsCollectionStyle}>
                    <MetricBlock name={"Temperature"} value={this.state.metrics.air_temperature.toFixed(0)} unit={"°F"}/>
                    <MetricBlock name={"RH"} value={this.state.metrics.air_relative_humidity.toFixed(0)} unit={"%"}/>
                    <MetricBlock name={"CO2"} value={this.state.metrics.air_co2_ppm.toFixed(0)} unit={"PPM"}/>
                </div>
                <h1>Water</h1>
                <div style={metricsCollectionStyle}>
                    <MetricBlock name={"pH"} value={this.state.metrics.pH.toFixed(1)} unit={""}/>
                    <MetricBlock name={"Level"} value={this.state.metrics.water_level.toFixed(1)} unit={"in"}/>
                    <MetricBlock name={"EC"} value={(this.state.metrics.electrical_conductivity / 1000).toFixed(2)} unit={"mS/cm²"}/>
                </div>
                <h1>Controls</h1>
                <div style={metricsCollectionStyle}>
                    <MetricBlock
                        name={"Pump"}
                        value={this.state.metrics.pump ? "ON" : "OFF"}
                        unit={this.state.metrics.pump ? "Turn OFF" : "Turn ON"}
                        onClick={() => {
                            httpPost("/api/togglePump", "").then((response) => {
                                if (response.status === 401) {
                                    // unauthorized
                                    reportInvalidToken();
                                }
                            });
                            this.setState({metrics: {...this.state.metrics, pump: !this.state.metrics.pump}});
                        }}
                    />
                    <MetricBlock
                        name={"Light"}
                        value={this.state.metrics.light ? "ON" : "OFF"}
                        unit={this.state.metrics.light ? "Turn OFF" : "Turn ON"}
                        onClick={() => {
                            httpPost("/api/toggleLight", "").then((response) => {
                                if (response.status === 401) {
                                    // unauthorized
                                    reportInvalidToken();
                                }
                            });
                            this.setState({metrics: {...this.state.metrics, light: !this.state.metrics.light}});
                        }}
                    />
                </div>
            </div>
        )
    }
}