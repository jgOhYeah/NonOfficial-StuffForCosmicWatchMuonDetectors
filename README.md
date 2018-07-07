# NonOfficial-StuffForCosmicWatchMuonDetectors
Extra code and stuff I (Jotham Gates) have made/adapted for Spencer Axiani's Cosmic Watch Desktop Muon Detector V2
This is a work in progress and there might be the odd bug and issue.

The official website can be found at [http://www.cosmicwatch.lns.mit.edu/](http://www.cosmicwatch.lns.mit.edu/)
The official github repository and resources can be found at [https://github.com/spenceraxani/CosmicWatch-Desktop-Muon-Detector-v2](https://github.com/spenceraxani/CosmicWatch-Desktop-Muon-Detector-v2)

## Arduino Sketch
The arduino sketch here is an attempt to get the oled display and the sd card to work at the same time.

Note that the current arduino code saves files in csv format. It should be possible to change this by commenting out the line "#define saveAsCSV".
It should also be possible to disable various functions such as saving to sd cards, driving the oled display and sending serial data to improve speed or memory usage by commenting out the "#define useSdCard", "#define useSerial", and or "#define useScreen" lines.

I have not tested this with the python scripts, but they might work.
I have not built a second detector yet, so I have not confirmed that the coincidence functionality works properly either.

## Muon Detector GUI
This is a smallish program written in Unity using the C# programming language. It displays live information from the detector.
The compiled files can be found in the folder named build. Please note that I have not tested the Macintosh version and it may or may not work.
The graphs are done using the Dynamic Line Chart asset package by Yun Shi. Various bits of the code (ie most of it) are copied from various tutorials and websites and merged together, although I can't remember exactly which bits of code were copied and where they came from.