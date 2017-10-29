# trafficLight
Code for a traffic light/timer on an arduino metro

This is the code for an Arduino-based traffic light/timer. Hardware is set up as follows:
-Red LED on pin 13
-Yellow LED on pin 12
-Green LED on pin 11
-push-buttons on pins 2 & 3
-toggle switch on pin 4
-seven-segment display with I2C backpack, address 0x70, for moderator
-seven-segment display with I2C backpack, address 0x71, for presenter

I used a metro mini, but any Arduino Uno-compatible board should work.

The timer has two modes. Manual mode allows the moderator to cycle through the phases at will.
Timer mode allows the moderator to set a time limit and initiate a countdown.

This project is licensed under the terms of the MIT license. See the license file for details.
