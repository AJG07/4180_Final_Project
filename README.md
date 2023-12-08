# 4180_Final_Project
**Team members:** Andrew Gunawan, Bo Pang, Tianle Li
## Project Overview
### Short Summary
For this project, we built an RC surveillance vehicle. The vehicle has a rotating camera and two wheels driven by their own motors. The user can use Xbox Controller to control the speed of each wheel and rotate the camera so it can capture visual data from different angles. The vehicle can transmit real-time videos feed to a network. It also has a braking system and a speaker so that it can stop when detecting obstacles and set alarm.
### Demo Video
## Hardware Platform
## Software Platform

**Components:**

the Xbox establishes a connection to the user's laptop via a 2.4G connection. The user utilizes a driver to acquire button information from the Xbox, where the values of **two joysticks** range from -1 to 1, and **two buttons** have binary states (pressed: 1, not pressed: 0). The driver processes the obtained Xbox information, logically interpreting user actions. The button values are then converted into a string and transmitted via TCP to the Raspberry Pi. The user sends data to the Pi at a rate of 30 transmissions per second.





### Software Block Diagram

![](./Pictures/software.jpg)



## Limitations / Possible Improvements

