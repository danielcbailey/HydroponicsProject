import React from 'react';
import {httpPost, setCookie} from "../util";
import '../App.css';

export class LoginPage extends React.Component {
    constructor(props) {
        super(props);

        this.tryLogin = this.tryLogin.bind(this);
        this.enterListener = this.enterListener.bind(this);

        // binding event listeners
        document.addEventListener("keydown", this.enterListener);

        this.state = {
            password: "",
            failed: false,
        }
    }

    tryLogin() {
        httpPost("/api/authenticate", this.state.password).then((response) => {
            if (response.status === 200) {
                response.text().then((token) => {
                    setCookie("token", token);
                    document.removeEventListener("keydown", this.enterListener);
                    this.props.onSuccess();
                });
            } else {
                this.setState({failed: true});
            }
        });
    }

    enterListener = (e) => {
        if (e.key === 'Enter') {
            this.tryLogin();
        }
    }

    render() {
        return (
            <div style={{
                display: "flex",
                flexDirection: "column",
                alignItems: "center",
                justifyContent: "center",
                width: "100%",
                height: "100%",

            }}>
                <div style={{
                    display: "flex",
                    flexDirection: "row",
                    alignItems: "center",
                    justifyContent: "center",
                    height: "100%",
                }}>
                    <div>
                        <div style={{
                            paddingBottom: 10,
                        }}>
                            <h1>WELCOME BACK</h1>
                            <p className={"body-text"}>Please enter your password to login.</p>
                        </div>
                        <input
                            className="text-input"
                            type="password"
                            placeholder="Password"
                            style={{width: 400}}
                            onChange={(e) => this.setState({password: e.target.value, failed: false})}
                        />
                        <button className="solid-button" onClick={() => this.tryLogin()}>Login</button>
                        {this.state.failed ? <p className="error-text">Incorrect password.</p> : null}
                    </div>
                </div>
            </div>
        );
    }
}