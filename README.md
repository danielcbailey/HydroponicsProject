# HydroponicsProject

Daniel Pierce Cooper

Deniz Lara Gunesen

## Overview

This project was to construct a semi-autonomous hydroponic farm for growing herbs without the need for pesticides year-round. The system has two controls that it can autonomously control: the light source and a water pump. These are the two activities that need to be performed most routinely, so the remaining actions of nutrient, pH, and water adjustments can be relegated to infrequent manual intervention. Although those manual requirements still exist, the system has the sensors to monitor these parameters so that the attendant can remotely check on the values and the system can suggest the amount of adjustment required.

### Contents

1. [Highlights Video](#highlights-video)
2. [System High-Level Diagram](#system-high-level-diagram)
3. [System Schematic](#system-schematic)
4. [Parts List](#parts-list)
5. [Construction Details](#construction-details)
6. [Programming Details](#programming-details)
7. [Microcontroller Operation](https://github.com/danielcbailey/HydroponicsProject/blob/main/docs/Microcontroller_operation.md)
8. [Microcontroller-Server Interface](https://github.com/danielcbailey/HydroponicsProject/blob/main/docs/Microcontroller-Server_Interface.md)
9. [Microcontroller User Interface](https://github.com/danielcbailey/HydroponicsProject/blob/main/docs/Microcontroller-User_Actions.md)

## Highlights Video

[![View on YouTube](https://img.youtube.com/vi/q4DIJgzh-Gc /0.jpg)](https://www.youtube.com/watch?v=q4DIJgzh-Gc )

## System High-Level Diagram

TODO: put the image.

A Raspberry Pi Pico is used for interfacing with the sensors, relays, and a small control panel for offline operation. A Raspberry Pi 4B is used as a server that hosts both the static website files, the backend server, and a Clickhouse database. These two computers communicate using USB Virtual Serial.

## System Schematic

TODO: put the image.

Not shown is the Raspberry Pi 4B, which the Pico's USB is connected to. The Raspberry Pi 4B has a USB flash drive for storing the database content.

## Parts List

- 1x Raspberry Pi Model 4B
- 1x Raspberry Pi Pico
- 1x Atlas Scientific pH Probe Kit
- 1x Atlas Scientific EC Probe K1.0 Kit
- 1x Adafruit SCD-30 NDIR CO2,Temperature, and Humidity Sensor
- 1x Adafruit TSL2591 High Dynamic Range Digital Light Sensor
- 1x 8 inch Milone eTape Water Level Sensor
- 1x 2 ch Relay Module
- 1x 396 GPH Submersible Pump
- 1x 150W LED Grow Light
- 1x Rotary Encoder with Push Button
- 1x 128x64 Graphic LCD

Parts list is only reflective of major components. Not included are small ICs for glue logic, resistors, electrical outlets, etc. For more details on how each is used, please visit the [microcontroller operation page](https://github.com/danielcbailey/HydroponicsProject/blob/main/docs/Microcontroller_operation.md).

## Construction Details

### Control Box

The control box houses the Raspberry Pi 4B, the Pico, and Atlas Scientific instrument boards. The box was 3D printed and is assembled using M2.5 bolts. These bolts are also used to secure the Pi 4B, Pico, and LCD. The box is frankly too small for the amount of wires connecting everything, so a box with more depth would be desired for replication. The Pico gets its power from the Pi 4B's USB port, and the Pi 4B gets power from wires attached directly to its 5V header pins, with a USB connector on the other end.

### Structure

The structure is made of normal construction-grade wood. The bottom has pressure-treated wood in case of small leaks. The structure is held together with normal wood screws. It isn't very sturdy but it gets the job done.

### Relay Box

The relay box houses all the high voltage AC wiring. It contains a relay module for 2 controlled outlets. There are also 2 always-on outlets for connecting accessories. The box itself is a 4-gang switch and outlet box that you would use in home construction, and the wire is an appliance extension cable with the end chopped off.

## Programming Details

### Microcontroller (Pico)

The Pico was programmed in C/C++ using the [Raspberry Pi Pico C API](https://github.com/raspberrypi/pico-sdk). Although there are now easier (and cheaper) ways of programming it, I've been using the VisualGDB extension to Visual Studio since the day the Pico launched so we continued to use that for this project aswell. The JSON message parsing/serialization was accomplished with the [cJSON C library](https://github.com/DaveGamble/cJSON). The Pico supports drag-and-drop programming, but the more powerful SWD programming interface was used with another RPI Pico with [Picoprobe](https://github.com/raspberrypi/picoprobe) flashed on it. For more information about the programming implementation for the microcontroller, please visit our [Microcontroller Operation](https://github.com/danielcbailey/HydroponicsProject/blob/main/docs/Microcontroller_operation.md), [Microcontroller-Server Interface](https://github.com/danielcbailey/HydroponicsProject/blob/main/docs/Microcontroller-Server_Interface.md), and [Microcontroller User Interface](https://github.com/danielcbailey/HydroponicsProject/blob/main/docs/Microcontroller-User_Actions.md) pages.

### Server (Pi 4B)

The backend server was programmed in Golang using the standard library for the HTTP server. It runs asynchronous tasks such as polling the sensor readings, updating the database, and sending the updated time. It also hosts the REST API for the frontend, and a websocket listener for realtime sensor readings.

### Frontend

The frontend was written with ReactJS in Javascript. As mentioned above, it communicates with the backend via REST API. It is hosted by the backend using Golang's standard FileServer implementation.