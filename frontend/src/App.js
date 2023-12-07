import React from 'react';
import './App.css';
import {getCookieByName} from "./util";
import {LoginPage} from "./login/loginPage";
import {ViewPage} from "./viewPage/viewPage";
import {initMetrics} from "./metrics";
import {SettingsPage} from "./settingsPage/settingsPage";

export class App extends React.Component {
    constructor(props) {
        super(props);

        let alreadySignedIn = getCookieByName('token') !== null;

        this.state = {
            page: alreadySignedIn ? "view": "login",
        }

        if (alreadySignedIn) {
            initMetrics();
        }
    }

    render() {
        let mainComponent = null;
        if (this.state.page === "login") {
            mainComponent = <LoginPage onSuccess={() => {
                this.setState({page: "view"});
                initMetrics();
            }}/>
        } else if (this.state.page === "view") {
            mainComponent = <ViewPage switchToSettings={() => this.setState({page: "settings"})}/>;
        } else if (this.state.page === "settings") {
            mainComponent = <SettingsPage goBack={() => this.setState({page: "view"})}/>;
        }

        return (
            <div style={{width: "100vw", height: "100vh"}}>
                {mainComponent}
            </div>
        );
    }
}

export default App;
