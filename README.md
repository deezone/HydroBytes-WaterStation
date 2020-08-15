# HydroBytes-waterManagement
A collection of systems using microcontrollers to manage water in an automated garden.

## Introduction

The "garden" is simply a backyard padio in Brooklyn, New York. Typically there are only two seasons - cold and hot in New York City. By automating an urban garden ideally the space will thrive with minimum supervision. The amount of effort to automate besides the point, everyone needs their vices.

![Garden Layout](https://raw.githubusercontent.com/deezone/HydroBytes-waterManagement/master/resources/HydroBytes-38-74-Garden.jpg)

## The Water Management System (Ministry of Plenty)

This repo is for the Water Management systems of the project. It's responsablities are:
- adding water to a water resorvour (rain barrel)
- monitor the water level in the water resorvour
- pumping water to the Delivery System (Ministry of Love) that waters and monitors the various foliage in the garden
- provide access and control of the subsystems through REST API requests
- send status and related readings to the Central System (Big Brother)


### Release Notes
Resources and thougbts on each release. Development details as well as links to help explain choices made in how the system was built.
- [v0.0.1 - 25 July 2020](https://github.com/deezone/HydroBytes-waterManagement/blob/master/resources/releases/v00-00-01.md)
Functional soluton to two controllers - Ardunio Uno and ESP8266 ESP-01 communicating through a serial connetion. The ESP prodives a wifi access point and the Uno works with all of the sensors.
- [v0.0.2 - 14 August 2020](https://github.com/deezone/HydroBytes-waterManagement/blob/master/resources/releases/v00-00-02.md)
Adds functional two way serial communication between Ardunio Uno and ESP8266 ESP-01 as a part of a GET /status 200 response.


**Note**: The naming of the systems is in reference to the novel **[1984](https://en.wikipedia.org/wiki/Nineteen_Eighty-Four)** *by George Orwell*. If you've not read it, you should! It's a classic that any self sufficent, automated garden loving nerd would enjoy.

The system names come from the [Ministries of Nineteen Eighty-Four](
https://en.wikipedia.org/wiki/Ministries_of_Nineteen_Eighty-Four).