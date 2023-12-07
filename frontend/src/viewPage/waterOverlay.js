import React from 'react';
import {addMetricsListener, getMetrics, removeMetricsListener} from "../metrics";
import {MetricBlock} from "./metricBlock";
import {httpGet, reportInvalidToken} from "../util";

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

export class WaterOverlay extends React.Component {
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

        this.state = {metrics: metrics, targets: {
            ph_min: 0,
            ph_max: 0,
            ec_min: 0,
            ec_max: 0,
        }, adjustments: {
            phUp: 0,
            phDown: 0,
            nutrients: 0,
        }};
    }

    componentDidMount() {
        addMetricsListener(this.metricsUpdate);
        httpGet('/api/getTargets').then((response) => {
            if (response.status === 200) {
                response.json().then((targets) => {
                    this.setState({targets: targets});
                });
            } else if (response.status === 401) {
                // unauthorized
                reportInvalidToken();
            }
        });
        httpGet('/api/getPHAdjustment').then((response) => {
           if (response.status === 200) {
               response.json().then((phAdjustments) => {
                   let adjustments = this.state.adjustments;
                   adjustments.phUp = phAdjustments.ph_up_ml.toFixed(0);
                   adjustments.phDown = phAdjustments.ph_down_ml.toFixed(0);
                   this.setState({adjustments: adjustments});
               });
           } else if (response.status === 401) {
               // unauthorized
               reportInvalidToken();
           }
        });
        httpGet('/api/getECAdjustment').then((response) => {
            if (response.status === 200) {
                response.json().then((phAdjustments) => {
                    let adjustments = this.state.adjustments;
                    adjustments.nutrients = phAdjustments.nutrient_ml.toFixed(0);
                    this.setState({adjustments: adjustments});
                });
            } else if (response.status === 401) {
                // unauthorized
                reportInvalidToken();
            }
        });
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
            <div style={{width: 'calc(100% - 150px)', height: '100%', position: 'relative', left: 150}}>
                <div style={{
                    position: 'absolute',
                    right: 50,
                    top: 0,
                    marginTop: 50
                }}>
                    <h1>Water</h1>
                    <div style={metricsCollectionStyle}>
                        <MetricBlock name={"pH"} value={this.state.metrics.pH.toFixed(1)} unit={""}/>
                        <MetricBlock name={"Level"} value={this.state.metrics.water_level.toFixed(1)} unit={"in"}/>
                        <MetricBlock name={"EC"} value={(this.state.metrics.electrical_conductivity / 1000).toFixed(2)} unit={"mS/cm²"}/>
                    </div>
                    <h1>Target pH</h1>
                    <div style={metricsCollectionStyle}>
                        <MetricBlock name={"Minimum"} value={this.state.targets.ph_min.toFixed(2)} unit={"pH"}/>
                        <MetricBlock name={"Maximum"} value={this.state.targets.ph_max.toFixed(2)} unit={"pH"}/>
                        <MetricBlock name={this.state.adjustments.phDown > 0 ? "pH Down Suggested" : "pH Up Suggested"} value={this.state.adjustments.phDown > 0 ? this.state.adjustments.phDown : this.state.adjustments.phUp} unit={"mL"}/>
                    </div>
                    <h1>Target EC</h1>
                    <div style={metricsCollectionStyle}>
                        <MetricBlock name={"Minimum"} value={this.state.targets.ec_min.toFixed(2)} unit={"mS/cm²"}/>
                        <MetricBlock name={"Maximum"} value={this.state.targets.ec_max.toFixed(2)} unit={"mS/cm²"}/>
                        <MetricBlock name={"Nutrients Suggested"} value={this.state.adjustments.nutrients} unit={"mL"}/>
                    </div>
                </div>
            </div>
        );
    }
}