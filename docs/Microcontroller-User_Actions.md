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

## 1. Sensor Readings

Sensor readings page display all of the readings from the sensors. There isn't any action that a user can perform from this page. The readings are updated real time. They can be used for plant monitoring purposes. For example, if the pH value has changed this might indicate that the concentration of the nutrient solution went down in the pipes. 

The Sensor Readings page displays all readings from the sensors. There are no user actions available on this page; the readings are updated in real-time. They serve the purpose of plant monitoring. For instance, a change in pH value indicates a decrease in nutrient solution concentration within the pipes.

![WhatsApp Image 2023-12-06 at 10 25 18 AM (2)](https://github.com/danielcbailey/HydroponicsProject/assets/32362546/f5908045-bf56-4d98-8b77-16f9a8776840)
Figure 4. Sensor Readings Page showing temperature, humidity, CO2, pH, electrical conductivity, water level in inches, and light intensity in lumen.


## 2. Light and Pump Control

The LED light and water pump are connected to 120 AC via relays. Therefore, it is crucial for the user to be able to manually control the on/off status of these outputs at any time. The user can independently activate or deactivate the lights and water pump. To mitigate the risk of fire hazards, the system checks the air temperature when the user attempts to turn on the lights. If the temperature exceeds the system's maximum value, the lights will not turn on. Similarly, when the user intends to turn on the pumps, the system examines the water level sensor readings. If the value is below a specified minimum water level limit, the pumps will not start, preventing water deprivation and potential damage to the pump.

![WhatsApp Image 2023-12-06 at 10 25 18 AM (3)](https://github.com/danielcbailey/HydroponicsProject/assets/32362546/230de66d-0d1c-45fc-b8a8-cb8fbebf59c0)
Figure 5. Manual Control Menu 

## 3. Schedule Display

This page displays all events from the latest schedule sent by the server. If the number of events exceeds the length of the LCD screen, user can rotate the knob to scroll through the schedule. However, it's important to note that the schedule cannot be modified from this page. Only the server has the capability to send a schedule to the Pico (refer to the server interaction document).

TODO add pic to a schedule menu 
Figure 6. Schedule Display Page 

## 4. pH Calibration

The Atlas Scientific pH sensor requires a 3-point calibration process at the start of operations and periodically thereafter. The pH Calibration menu displays three selections corresponding to the three calibration solutions that come with the sensor (7.0 pH - mid, 4.0 pH - low, 10.0 pH - high). It is crucial that the user places the sensor in the solution before selecting its respective page on the pH calibration selection menu. For instance, when the user immerses the sensor in a 7.0 pH solution and selects the "1 - MID (7.0)" option, the system will sample the pH readings using a circular buffer. The mean and standard deviation of this circular buffer will then be calculated. Once these values stabilize, the Pico will send the calibration command to the pH sensor via UART.

Even if the sensor stabilizes at 6.956 pH, the calibration method ensures it reads precisely 7.0 pH from the calibration solution. This process must be repeated for the low and high calibration points as well.
The calibration processes are detailed in the datasheet, which can be found on this page:  https://atlas-scientific.com/kits/ph-kit/

![WhatsApp Image 2023-12-06 at 10 25 18 AM (4)](https://github.com/danielcbailey/HydroponicsProject/assets/32362546/67bc6370-cafb-4d6e-a549-5e1b39e9cc57)
Figure 7. pH Calibration Selection Menu 

TO DO 
Figure 8. MID-POINT Calibration Page during the calibration 

TO DO 
Figure 9. MID-POINT Calibration Page after the calibration is successfully performed 

## 5. EC Calibration



