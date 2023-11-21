# Microcontroller-Sever Inferface

The microcontroller and server communicate with USB virtual serial. For the microcontroller,
this will be exposed via stdin/stdout and for the server, this will be a file-system mapped
entity. The two will communicate at a virtual baud rate of 115200 bits/s. The protocol will
be JSON-RPC 2.0.

## Message Types

Messages consist of serialized JSON strings, and separated by newline character ('\n').
Always include a newline character after each message sent so that the other party knows
to start parsing its buffered data.

### Request (Server to Microcontroller)

Will be in the following format:
```json
{
    "jsonrpc": "2.0",
    "method": "some/remote/proceedure",
    "params": {
        "field1": 123,
        "field2": true,
        "field3": "test"
    },
    "id": 1
}
```

### Response (Microcontroller to Server Solicited)

Will be in the following format on successful completetion:
```json
{
    "jsonrpc": "2.0",
    "id": 1,
    "result": {
        "some": "fields"
    }
}
```

Otherwise, should be in the following format for an error:
```json
{
    "jsonrpc": "2.0",
    "id": 1,
    "error": {
        "code": 123,
        "message": "human readable info and/or additional context",
        "data": {
            "optional": "object"
        }
    }
}
```

### Notification (Microcontroller to Server Unsolicited)

The microcontroller can notify the server asynchronously for things such as log events,
errors that occur during autonomous interactions, or human interactions (such as calibration).

Format:
```json
{
    "jsonrpc": "2.0",
    "method": "some/event",
    "params": {
        "field1": 123,
        "field2": true,
        "field3": "test"
    }
}
```

## Microcontroller Methods

These methods are expected by the server for the microcontroller to fulfill.

### Set Time

Time is in seconds since midnight Jan 1, 1970 (unix epoch time). The server will send this
every hour.

NOTE: in some cases, time may be sent back if the microcontroller trends faster than
real time, in this case, schedule-driven events can be repeated as none would
detrimental to repeat for the margin of error that could be attributed to drift (few seconds).

If however, time jumps so far that it skips events, it should detect the last event for a
given subcategory (like LIGHTS or PUMP) and set the state to the last value (ON or OFF).

Example Request:
```json
{
    "jsonrpc": "2.0",
    "method": "config/settime",
    "params": {
        "unixtime": 1700584812
    },
    "id": 1
}
```

Response (Acknowledge):
```json
{
    "jsonrpc": "2.0",
    "id": 1,
    "result": {}
}
```

### Set Schedule

The 24-hour schedule the microcontroller should follow from this point forward until
it either gets power cycled or gets a new schedule sent to it. All schedules start at
0000 (12:00am) and finish at 2359 (11:59pm). All times expressed in the schedule are
in 24-hour notation HHMM (leading zeros removed). Similar to set time, there are some
stipulations for how the current schedule transitions to the incoming one. Like set
time, it should figure out what the state of each control should be based on the last
event for each before the current time.

Example Request:
```json
{
    "jsonrpc": "2.0",
    "method": "config/setschedule",
    "params": {
        "schedule": [
            {
                "event_type": "PUMP_ON",
                "event_time": 500
            },
            {
                "event_type": "PUMP_OFF",
                "event_time": 510
            }
        ]
    },
    "id": 1
}
```

Response (Acknowledge):
```json
{
    "jsonrpc": "2.0",
    "id": 1,
    "result": {}
}
```

### Read Sensors

A request to obtain all the most recent sensor readings. The server will call this
every minute and can be used as a heart beat to track connection.

Example Request:
```json
{
    "jsonrpc": "2.0",
    "method": "sensors/read",
    "params": {},
    "id": 1
}
```

Example Response:
```json
{
    "jsonrpc": "2.0",
    "id": 1,
    "result": {
        "water_level": 5.632,
        "cumulative_liquid_flow": 27846.2,
        "current_liquid_flow": 23,
        "light_intensity": 120,
        "pH": 7.3,
        "electrical_conductivity": 23000,
        "air_temperature": 23.2,
        "air_relative_humidity": 45.6,
        "air_co2_ppm": 670
    }
}
```

Units:
| Parameter               | Unit                 |
| ----------------------- | -------------------- |
| water_level             | inches (from bottom) |
| cumulative_liquid_flow  | liters (since boot)  |
| current_liquid_flow     | ml per second        |
| light_intensity         | lux                  |
| pH                      | pH 0-14              |
| electrical_conductivity | uS/cm                |
| air_temperature         | degrees celcius      |
| air_relative_humidity   | % RH                 |
| air_co2_ppm             | parts per million    |

### Set Pump State

A request to set the state of the pump to either ON (true) or OFF (false).

Example Request:
```json
{
    "jsonrpc": "2.0",
    "method": "controls/setpump",
    "params": {
        "state": true
    },
    "id": 1
}
```

Response (Acknowledge):
```json
{
    "jsonrpc": "2.0",
    "id": 1,
    "result": {}
}
```

### Set Light State

A request to set the state of the light to either ON (true) or OFF (false).

Example Request:
```json
{
    "jsonrpc": "2.0",
    "method": "controls/setlight",
    "params": {
        "state": true
    },
    "id": 1
}
```

Response (Acknowledge):
```json
{
    "jsonrpc": "2.0",
    "id": 1,
    "result": {}
}
```

## Events Available to the Microcontroller

Events (notifications) can be sent at any time by the microcontroller to the server.

### Log Message

A message to be logged for record keeping and/or debugging. There is a type field for predefined
types. If the log message is best represented with that type, use it. Any message you provide in
this case will be additional context.

Example Event:
```json
{
    "jsonrpc": "2.0",
    "method": "event/log",
    "params": {
        "severity": "debug/info/warn/error",
        "message": "this is some text",
        "message_type": "optional field, see below"
    }
}
```

Message Types:
- USER_INTERACTION: the user interacted with the device directly
- STATE_CHANGED: the user manually changed the state of the machine
- CONFIGURATION_CHANGED: the system configuration changed, such as calibration
- HARDWARE_FAILURE: the machine was unable to confirm an action resulted in state change, or
    failed to read from a sensor.
- REBOOT: the microcontroller rebooted, reason should be specified in the message