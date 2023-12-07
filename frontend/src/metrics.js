import {getCookieByName, reportInvalidToken} from "./util";

var listeners = [];
var lastMetrics = null;

export function initMetrics() {
    // opens a websocket connection to the server that gets data
    // eslint-disable-next-line no-restricted-globals
    let socket = new WebSocket("ws://dpcooper.me:8088/api/dataWebsocket?token=" + getCookieByName("token"));
    socket.onmessage = (event) => {
        let data = JSON.parse(event.data);
        lastMetrics = data;
        console.log("Got metrics: ", data);
        listeners.forEach((listener) => listener(data));
    };
    socket.onerror = (event) => {
        console.log(event);
        reportInvalidToken();
    };
}

export function getMetrics() {
    return lastMetrics;
}

export function addMetricsListener(listener) {
    // adds a listener that gets called when new data is received
    listeners.push(listener);
}

export function removeMetricsListener(listener) {
    // removes a listener
    listeners = listeners.filter((l) => l !== listener);
}