# Microcontroller Operation

## Summary

The microcontroller is responsible for the day-to-day operation of the machine. This includes:
- Reading sensor values
- Controlling the light and pump
- Following a schedule for the operation of the pump and light

The microcontroller should be able to operate completely independently of the rest of the system and
should maintain the system. This includes:
- Follow a default schedule on power up
- Follow the schedule to control the pump and light
- Manually control the pump and light from the machine
- Calibrate any sensor that requires calibration upon replacement (pH, EC, light intensity)

## User Interaction

The user interacts with the microcontroller directly via a 128x64 graphic LCD and a rotary encoder.
This is primarly as a backup to the web-based interaction path but in the event the web page fails or
the user desires not to have it up while performing maintainence, it exists and allows for the bullets
described above.

## Sensors

### pH Sensor

Used for monitoring water quality and estimating nutrient levels. Communicates to the microcontroller
using UART. For more details, please see:

[Atlas Scientific pH Sensor](https://atlas-scientific.com/kits/ph-kit/)

### EC (Electrical Conductivity) Sensor

Used for monitoring water quality and estimating nutrient levels. Communicates to the microcontroller
using UART. For more details, please see:

[Atlas Scientific EC Sensor](https://atlas-scientific.com/kits/conductivity-k-1-0-kit/)

### Light Intensity

Used for monitoring the health of the grow light. Lower output or complete lack of output indicate
imminent or complete failure. The sensor uses I2C to communicate with the microcontroller. For more
details, please see:

[Adafruit Digital Light Sensor](https://www.adafruit.com/product/1980)

### Liquid Level Sensor

Used to monitoring the liquid levels in the water tank. If the level is too low, it should not run
the pump to protect it from damage. Also, for use in tandem with the flow sensor to check for leaks.
The sensor is a temperature-compensating resistive strip that varies with liquid level. This is fed
into the microcontroller's two analog inputs from voltage dividers using the sensor. For more details,
please see:

[8in eTape Level Sensor](https://www.adafruit.com/product/463)

### Liquid Flow Sensor

Used for monitoring pump health and checking for leaks. If the pump relay is on, but no flow, then
either the flow sensor or the pump failed and this would require service. For more details, please
see:

[Adafruit Liquid Flow Sensor](https://www.adafruit.com/product/828)

### Air Quality Sensor

Used for monitoring air temperature, humidity, and CO2 levels. Should any of these become out of
the acceptable range, the user will need to take action to protect the plants. CO2 levels should
not see much of a problem, but may result from lack of proper air circulation. For more details,
please see:

[Adafruit NDIR CO2 Sensor with Temperature and Humidity Compensation](https://www.adafruit.com/product/4867)

## Schedules

Since the microntroller should be able to drive the system autonomously, it needs a simple mechanism
to fall back on when the more elaborate server is unavailable to direct it. Schedules are how the
microcontroller manages the two controls it has to influence the system (pump and light). Schedules
are comprised of events and these events have both a type and a time. Schedules repeat every 24 hours.
The server can and will upload a new schedule shortly after boot that is fine-tuned to the specific
plants being grown.

The time should be obtained from the built-in realtime clock. The realtime clock will drift without
periodic refresh by the internet-connected server but this is acceptable for the purposes of a backup.

Below are a summary of the types of events:
- PUMP_ON (1): turns the pump on
- PUMP_OFF (2): turns the pump off
- LIGHT_ON (3): turns the light on
- LIGHT_OFF (4): turns the light off

The schedule has a resolution of one minute.

Below is the default schedule at boot (before overridden by the server):
| EVENT_TYPE | EVENT_TIME (HHMM) |
| ---------- | ----------------- |
| PUMP_ON    | 0500              |
| PUMP_OFF   | 0510              |
| LIGHT_ON   | 0600              |
| PUMP_ON    | 1100              |
| PUMP_OFF   | 1110              |
| PUMP_ON    | 1700              |
| PUMP_OFF   | 1710              |
| LIGHT_OFF  | 2000              |
| PUMP_ON    | 2300              |
| PUMP_OFF   | 2310              |

## Server-Microntroller Interaction

Please see the page for details of these interactions, but commands are sent over USB virtual serial and is exposed as stdin/stdout to the microcontroller.