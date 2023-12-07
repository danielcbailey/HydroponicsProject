import React from 'react';
import "../App.css";

let labelStyle = {
    width: 300,
    fontFamily: 'D-DIN-Bold',
};

let descriptionStyle = {
    width: 300,
    fontSize: 16,
    color: '#A0A0A0',
};

let unitStyle = {
    color: '#A0A0A0',
    width: 280,
    marginLeft: -300,
    textAlign: 'right',
    display: 'inline-block',
    pointerEvents: 'none',
}

export class SettingsTextBox extends React.Component {
    render() {
        return (
            <div style={{display: 'flex', flexDirection: 'row', alignItems: 'flex-start', gap: 10}}>
                <div>
                    <span className={"body-text"} style={labelStyle}>{this.props.name}</span>
                    <p className={"body-text"} style={descriptionStyle}>{this.props.description}</p>
                </div>
                <div style={{paddingTop: 5}}>
                    <input type={"text"} className={"text-input"} style={{width: 300}} value={this.props.value} onChange={(e) => {
                        this.props.onChange(e.target.value);
                    }}/>
                    <span style={unitStyle} className={"body-text"}>{this.props.unit}</span>
                    {this.props.error ? <p className={"error-text"}>{this.props.error}</p> : null}
                </div>
            </div>
        );
    }
}