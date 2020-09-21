# HydroBytes-WaterManagement
A collection of systems using micro controllers to manage water in an automated garden.

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
- pumping water to the Delivery System (Ministry of Love) that waters and monitors the various foliage in the garden
- provide access and control of the subsystems through REST API requests
- send status and related readings to the Central System


### Release Notes
Resources and thoughts on each release. Development details as well as links to help explain choices made in how the system was built.
- [v0.1.0 - 30 August 2020](https://github.com/deezone/HydroBytes-waterManagement/blob/master/resources/releases/v00-01-00.md)
Water level detection on water barrel.
- [v0.0.3 - 15 August 2020](https://github.com/deezone/HydroBytes-waterManagement/blob/master/resources/releases/v00-00-03.md)
Working proof of concept. LED control and monitoring of state with http requests.
- [v0.0.2 - 14 August 2020](https://github.com/deezone/HydroBytes-waterManagement/blob/master/resources/releases/v00-00-02.md)
Adds functional two way serial communication between Ardunio Uno and ESP8266 ESP-01 as a part of a GET /status 200 response.
- [v0.0.1 - 25 July 2020](https://github.com/deezone/HydroBytes-waterManagement/blob/master/resources/releases/v00-00-01.md)
Functional solution to two controllers - Ardunio Uno and ESP8266 ESP-01 communicating through a serial connection. The ESP provides a WiFi access point and the Uno works with all of the sensors.
