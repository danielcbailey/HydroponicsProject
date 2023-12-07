

// gets the value of a cookie or null if not found
export function getCookieByName(name) {
    var value = "; " + document.cookie;
    var parts = value.split("; " + name + "=");

    if (parts.length === 2) {
        return parts.pop().split(";").shift();
    }

    return null;
}

// sets the value of a cookie
export function setCookie(name, value, days = 1) {
    var expires = "";
    if (days) {
        var date = new Date();
        date.setTime(date.getTime() + (days*24*60*60*1000));
        expires = "; expires=" + date.toUTCString();
    }
    document.cookie = name + "=" + (value || "")  + expires + "; path=/";
}

// deletes a cookie
export function deleteCookie(name) {
    document.cookie = name + '=; expires=Thu, 01 Jan 1970 00:00:01 GMT;';
}

// performs a GET request to the specified URL, returns a promise
export function httpGet(url) {
    return fetch(url, {
        method: 'GET',
        headers: {
            'Content-Type': 'application/json',
            'Authorization': getCookieByName('token')
        }
    });
}

// performs a POST request to the specified URL, returns a promise
export function httpPost(url, data) {
    return fetch(url, {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
            'Authorization': getCookieByName('token')
        },
        body: data
    });
}

export function reportInvalidToken() {
    deleteCookie('token');
    window.location.reload();
}

export function getSimpleDuration(durationMs) {
    // durationMs is in milliseconds
    // figures out whether it is best represented in days, or weeks
    let days = durationMs / (1000 * 60 * 60 * 24);
    let weeks = days / 7;

    if (Math.abs(days) > 14) {
        return {value: Math.round(weeks), unit: 'weeks'};
    }
    return {value: Math.round(days), unit: " days"};
}