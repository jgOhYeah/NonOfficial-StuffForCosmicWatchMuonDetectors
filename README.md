# NonOfficial-StuffForCosmicWatchMuonDetectors
Extra code and stuff I (Jotham Gates) have made/adapted for Spencer Axiani's Cosmic Watch Desktop Muon Detector V2

The official website can be found at [http://www.cosmicwatch.lns.mit.edu/](http://www.cosmicwatch.lns.mit.edu/)
The official github repository and resources can be found at [https://github.com/spenceraxani/CosmicWatch-Desktop-Muon-Detector-v2](https://github.com/spenceraxani/CosmicWatch-Desktop-Muon-Detector-v2)

## Arduino Sketch (Arduino Code/MuonDetector)
This is an alternate firmware for the muon detectors that allows the use of the oled display and sd card at the same time.

This software mostly works with the python scripts and server, although it does not support reading and deleting files off the sd card and the scripts do get the detector id wrong as it is in a different location to the original code.

This firmware also has a built in setings editor that can save values including screen brightness, detector ids and logging file formats into EEPROM without the need for the naming sketch. These settings will persist between updloads to the arduino.

#### Warning about ram usage:
Driving the display and sd card takes a lot of ram, especially during the init() function. If you are trying to edit this and are getting really weird results and behaviour in other areas of code / restarts. Running out of ram is likely to be the issue. Simply adding and removing debugging commands or changing the order that stuff is called was enough to stuff things up for me at times. Good luck.

### To save values and settings in EEPROM:
With the serial terminal set to "\\n" as the line ending character (new line), type "Settings" before the detector fully starts up and begins counting events.

In this version of code the ID is is saved as two parts - one for one line on the display and the other for the one below. In files and serial, these parts get concatenated. Add a space to the end of the first part if you want a space between them without effecting how it displays on the oled screen.
Each id can be up to 40 chars long but only the first 16 will be displayed on the screen on startup.

From there, type "ID1 " then the name for the first part of the detector id or "ID2 " then name for the second part of the detector id or "Format " then ".csv" or ".txt" to select the format to save to the sd card. - Changing this will have no effect if useSdCard is not defined (commented out) in defines.h. Use "Contrast" and values between 0 and 255 to set the screen brightness. On some displays 0 is off and in others it is the dimmest.

If you happen to make a mistake when typing in a setting to change and are having problems changing anything else, either reset the detector and reenter the settings menu or try typing something along the lines of "x x " or "dd  cv vvxcsd cx" to the the light to come on again (or go off, then type again to make it turn on). - This is due to a slight issue with how the program accepts text. The code for this bit works, but isn't the tidiest and probably needs to be fixed at some stage.

#### Images
Splash Screen:
![Splash Screen](./Example%20Data/Screens/SplashScreen.JPG)

SD Card and Master / Slave Screen:
![SD Card Screen](./Example%20Data/Screens/SDScreen.JPG)

Ready Screen:
![SD Card Screen](./Example%20Data/Screens/ReadyScreen.JPG)

Normal Running Screen:
![SD Card Screen](./Example%20Data/Screens/NormScreen.JPG)

## Bulk Settings Arduino Sketch (Arduino Code/bulkSettings)
Use to load the default settings into eeprom if you have a few detectors to program or don't want to use the built in settings menu.

Similar to the official naming.ino sketch except supports the second line of the detector ID, the file format and the brightness of the display.

To use, edit the #defines in the top of the file then compile and upload. When the Arduino next starts up, it will copy the values into EEPROM.

## Muon Detector GUI
This is a smallish program written in Unity using the C# programming language. It displays live information from the detector. - I would probably recommend the [official online version](http://cosmicwatch.lns.mit.edu/measure) combined with one of the server programs if you have internet access.
The compiled files can be found in the folder named build. Please note that I have not tested the Macintosh version and it may or may not work.
The graphs are done using the Dynamic Line Chart asset package by Yun Shi. Various bits of the code are copied from various tutorials and websites and merged together, although I can't remember exactly which bits of code were copied and where they came from.

# Changelog
* **12/08/2019 V2.3.2**
	* Made the bar graph graphical instead of hyphens.
	* Bug fixes:
		* Fixed printing signed integers for the time and count codes, so negative numbers hopefully don't start appearing after around 25 days running.
		* Fixed scrambling of the detector's ID and "Cosmic Watch" when talking to a computer.
		* Fixed a few memory issues when dealing with strings and probable created a few more.
		* A few more that I have forgotten about.
