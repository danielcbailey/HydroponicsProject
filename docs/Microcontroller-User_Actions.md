# User Actions

## Main Page

The main page displays crucial information to the user. Firstly, it indicates "CONNECTED" to signify a stable connection between the server and the pico. This is very important for the continuation of operations. Subsequently, it presents the date and time. Accuracy in time representation is crucial since the pico may sometimes acquire incorrect time from RTC.h. The server will periodically send the correct time to the Pico to ensure synchronization with the daily schedule of events. 

The page also features two essential sensor readings: Water level and Temperature. Monitoring these readings is imperative to ensure the safety and functionality of the system.

![PXL_20231205_201223478](https://github.com/danielcbailey/HydroponicsProject/assets/32362546/3740eab4-01f4-4710-aee2-7ce9ea7cf673)
Figure 1. Main page when pico boots up before the Pi connects to it

![PXL_20231205_212641890](https://github.com/danielcbailey/HydroponicsProject/assets/32362546/3fc9fdbc-b1d9-4c62-b719-c927d7ff0ed6)
Figure 2. Main page after Pi connects to the pico and updates time and schedule.

## Navigation Menu
From the main page, users can press the rotary encoder knob to access the navigation menu. This menu contains various manual actions that users can perform to monitor both plants and the system as needed.

![WhatsApp Image 2023-12-06 at 10 25 18 AM (1)](https://github.com/danielcbailey/HydroponicsProject/assets/32362546/f0b7e7eb-6f74-46d9-91c3-c288790a53d7)
Figure 3. Navigation Menu 


-- check below 
## Sensor Readings

Sensor readings page display all of the readings from the sensors. There isn't any action that a user can perform from this page. The readings are updated real time. They can be used for plant monitoring purposes. For example, if the pH value has changed this might indicate that the concentration of the nutrient solution went down in the pipes. 

## Light and Pump Control

LED light and water pump are connected to 120 AC via relays. So it is safety critical that user is able to turn on and off the outputs manually anytime. User can turn on and off the lights and the water pump independently. To prevent a fire hazard, when the user selects to turn on the lights, the system will check the air temperature. If the temperature is higher than the maximum temperature value of the system, the lights will not turn on. Similarly, when the user wants to turn on the pumps, the system checks the water level sensor readings. If the value is lower than a minimum water level limit, the pumps will not turn on. This prevents the water pump from being water deprived and getting damaged. 



