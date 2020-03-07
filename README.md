# oled

This repo contains three different approaches for writing info to the 0.96" OLED display in the subdirectories listed below:

1. "python" a python version, simplest, 110MB container, 6 lines of 20 chars
2. "cv1" a C version, 6.6MB container, 1 font, 3 lines of 16 chars
3. "cv2" a better C version, 6.1MB container, 3 fonts, 8 lines of 21 chars

Each of these provides various bits of information about its host, but the latter provides the most. All of them provide the IPv4 address of the default host interface, "live" gateway and internet connectivity status, "live" UTC time, and "live" uptime, and load average.

The latter however also provides "live" Open-Horizon status (whether it is installed, what is the `configstate.state` of the node, and policy or pattern registration details. All of this Open-Horizon info is updated dynamically ever 2 seconds. An example screenshot is shown below:

![](https://raw.githubusercontent.com/MegaMosquito/oled/master/cv2.png)
![](https://www.raspberrypi-spy.co.uk/wp-content/uploads/2012/06/raspberry_pi_3_model_b_plus_gpio.jpg)

Usage:

- install an I2C OLED display (wired to +3.3V, GND, SDA, and SCL)
- use raspi-config to enable I2C interface
- install docker and git
- git clone this repo, and cd into it
- cd into the implementation subdirectory of your choice, e.g., "cd cv2"
- run "make"

2019-11-15
Written by mosquito@darlingevil.com
