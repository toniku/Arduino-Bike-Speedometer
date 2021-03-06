# Arduino-Bike-Speedometer
Arduino based software school project // Bike speedometer

The purpose of this project was to create a bicycle speedometer. The device would display necessary information on an LCD screen and include some more advanced features.

For the product, we had to get input from the bicycle as well as from the user, compute all calculations and create a menu to scroll through different values on the LCD (distance, duration, clock, average speed, settings...).

The speedometer should also include features like data logging to an SD card, the ability to create different logging sessions, permanent storage of values in EEPROM memory and settings allowing the user to adjust the wheel size.

The code mainly consists of three parts:
  - Main loop polling the buttons used for scrolling through the display menu
  - A processor interrupt at every wheel spin to get the bicycle speed
  - A processor interrupt every second triggering all calculations and data logging

Time proved to be the most challenging resource to manage, and many other features could have been implemented to bring this product to the next level.
