This is still a Work In Progress! There may be a few bugs.

Non Official Version of the CosmicWatch Desktop Muon Detector Arduino Code

The original code and documentation / designs can be found here:
https://github.com/spenceraxani/CosmicWatch-Desktop-Muon-Detector-v2

This software uses both the oled display and the sd card if they are fitted.
Differences to the original version are mainly:
 - Using a smaller library for the driving the display to save RAM.
 - By default saves files in CSV format so can be opened in excel without having to be imported
 - Tries to use char arrays instead of the String library to save RAM.
 - Does not fully support the python scripts for connecting these devices to a computer.

Uses a different library by Bill Grieman for the driving the display than the official version that is much smaller, although possibly a bit slower
It can be downloaded here: https://github.com/greiman/SSD1306Ascii

Put all .ino and .h files in this folder in a directory titled "SDAndOled"
The defines.h file contains parameters that can be adjusted to adapt this to your needs and preferences.
The strings.h file contains most of the strings in this program, so change these if you want to change the wording.
 - You might have to fiddle around a bit with oled.setCursor(x,y); at various parts of this program if the spacing goes weird.
