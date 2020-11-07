# HydroBytes-WaterManagement
A collection of systems using micro controllers to manage water in an automated garden. The Water Management system is a part of a larger automated garden project called "[HydroBytes](https://github.com/deezone/HydroBytes)".

## Introduction

The "garden" is simply a backyard patio in Brooklyn, New York. Typically there are only two seasons - cold and hot in New York City. By automating an urban garden ideally the space will thrive with minimum supervision. The amount of effort to automate is besides the point, everyone needs their vices.

- **Water Management**
- **[Base Station](https://github.com/deezone/HydroBytes-BaseStation)**

![Garden Layout](https://raw.githubusercontent.com/deezone/HydroBytes-waterManagement/master/resources/HydroBytes-38-74-Garden.jpg)

![Garden](https://github.com/deezone/HydroBytes-WaterManagement/blob/master/resources/garden-01.png)

### YouTube Channel

[![YouTube Channel](https://github.com/deezone/HydroBytes-WaterManagement/blob/master/resources/youTube-TN.png?raw=true)](https://www.youtube.com/channel/UC00A_lEJD2Hcy9bw6UuoUBA "All of the HydroBytes videos")

## The Water Management System

This repo is for the Water Management systems of the project. It's responsibilities are:
- adding water to a water reservoir (rain barrel)
- monitor the water level in the water reservoir
- irrigate the garden. Sensor systems in the garden will coordinate with the [Base Station](https://github.com/deezone/HydroBytes-BaseStation) to determine when the Water Station should irrigate and for how long.
- provide access and control of the subsystems through REST API requests
- send status and related readings to the [Base Station](https://github.com/deezone/HydroBytes-BaseStation)


### Release Notes
Resources and thoughts on each release. Development details as well as links to help explain choices made in how the system was built.

- [v0.2.2 - 14 November 2020](https://github.com/deezone/HydroBytes-waterManagement/blob/master/resources/releases/v00-02-02.md)
Adds duration support to indicate how long an irriagtion has been running or how long it's been since the last irrigation.
- [v0.2.1 - 24 October 2020](https://github.com/deezone/HydroBytes-waterManagement/blob/master/resources/releases/v00-02-01.md)
Adds duration  GET /water responses to indicate the amount of time since the water pump state has been toggled.
- [v0.2.0 - 18 October 2020](https://github.com/deezone/HydroBytes-waterManagement/blob/master/resources/releases/v00-02-00.md)
Water irrigation support. A request to irrigate the garden includes monitoring the current water level available.
- [v0.1.1 - 27 September 2020](https://github.com/deezone/HydroBytes-waterManagement/blob/master/resources/releases/v00-01-01.md)
Addition of YouTube channel as well as general release notes clean up to include images and videos to help document the project.
- [v0.1.0 - 30 August 2020](https://github.com/deezone/HydroBytes-waterManagement/blob/master/resources/releases/v00-01-00.md)
Water level detection on water barrel.
- [v0.0.3 - 15 August 2020](https://github.com/deezone/HydroBytes-waterManagement/blob/master/resources/releases/v00-00-03.md)
Working proof of concept. LED control and monitoring of state with http requests.
- [v0.0.2 - 14 August 2020](https://github.com/deezone/HydroBytes-waterManagement/blob/master/resources/releases/v00-00-02.md)
Adds functional two way serial communication between Ardunio Uno and ESP8266 ESP-01 as a part of a GET /status 200 response.
- [v0.0.1 - 25 July 2020](https://github.com/deezone/HydroBytes-waterManagement/blob/master/resources/releases/v00-00-01.md)
Functional solution to two controllers - Arduino Uno and ESP8266 ESP-01 communicating through a serial connection. The ESP provides a WiFi access point and the Uno works with all of the sensors.
