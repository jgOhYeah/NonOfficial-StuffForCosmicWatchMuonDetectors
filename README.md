# NonOfficial-StuffForCosmicWatchMuonDetectors
Extra code and stuff I (Jotham Gates) have made/adapted for Spencer Axiani's Cosmic Watch Desktop Muon Detector V2
This is a work in progress and there might be the odd bug and issue.

The official website can be found at [http://www.cosmicwatch.lns.mit.edu/](http://www.cosmicwatch.lns.mit.edu/)
The official github repository and resources can be found at [https://github.com/spenceraxani/CosmicWatch-Desktop-Muon-Detector-v2](https://github.com/spenceraxani/CosmicWatch-Desktop-Muon-Detector-v2)

## Arduino Sketch (OledAndSD5thAttempt)
The arduino sketch here is an attempt to get the oled display and the sd card to work at the same time.


It should be possible to disable various functions such as saving to sd cards, driving the oled display and sending serial data to improve speed or memory usage by commenting out the "#define useSdCard", "#define useSerial", and or "#define useScreen" lines.

This software mostly works with the python scripts and server, although it does not support reading and deleting files off the sd card and the scripts do get the detector id wrong as it is in a different location to the original code.

The latest code now reads the detector id from EEPROM memory. In this version is is saved as two parts - one for one line on the display and the other for the one below. In files and serial, these parts get concatenated. - Add a space to the end of the first part if you want a space between them without effecting how it displays on the oled screen.

### To save values and settings in EEPROM:
With the serial terminal set to "\\n" as the line ending character, type "Settings" before the detector fully starts up and begins counting events.

From there, type "ID1 " then the name for the first part of the detector id or "ID2 " then name for the second part of the detector id or "Format " then ".csv" or ".txt" to select the format to save to the sd card. - Changing this will have no effect if useSdCard is not defined in defines.h.

If you happen to make a mistake when typing in a setting to change and are having problems changing anything else, either reset the detector and reenter the settings menu or try typing something along the lines of "x x " to the the light to come on again (or go off, then type again to make it turn on). - This is due to a slight issue with how the program accepts text.

## Bulk Settings Arduino Sketch
Use to load the default settings into eeprom if you have a few detectors to program or don't want to use the built in settings menu.

Similar to the official naming.ino sketch except supports the second line of the detector ID and the file format.

To use, edit the #defines in the top of the file then compile and upload. When the Arduino next starts up, it will copy the values into EEPROM.
## Muon Detector GUI
This is a smallish program written in Unity using the C# programming language. It displays live information from the detector. - I would probably recommend the [official online version](http://cosmicwatch.lns.mit.edu/measure) if you have internet access.
The compiled files can be found in the folder named build. Please note that I have not tested the Macintosh version and it may or may not work.
The graphs are done using the Dynamic Line Chart asset package by Yun Shi. Various bits of the code (ie most of it) are copied from various tutorials and websites and merged together, although I can't remember exactly which bits of code were copied and where they came from.