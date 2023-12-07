import React from 'react';
import "../App.css";

let metricSubBlockStyle = {
    display: 'flex',
    flexDirection: 'row',
    alignItems: 'flex-end',
    justifyContent: 'center',
}

let numberStyle = {
    fontSize: 60,
    fontFamily: 'D-DIN-Bold',
    color: 'white'
}

let unitStyle = {
    color: '#A0A0A0',
    paddingBottom: 6,
}

export class MetricBlock extends React.Component {
    render() {
        return (
            <div style={{textAlign: 'center', cursor: this.props.onClick ? 'pointer': null}} onClick={this.props.onClick}>
                <div style={metricSubBlockStyle}>
                    <span style={numberStyle}>{this.props.value}</span>
                    <span className={"body-text"} style={unitStyle}>{this.props.unit}</span>
                </div>
                <span className={"body-text"}>{this.props.name}</span>
                {this.props.error ? <p className={"error-text"}>{this.props.error}</p> : null}
            </div>
        );
    }
}