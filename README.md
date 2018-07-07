# NonOfficial-StuffForCosmicWatchMuonDetectors
Extra code and stuff I have made/adapted for Spencer Axiani's Cosmic Watch Desktop Muon Detector V2
This is a work in progress and there might be the odd bug and issue.

#Arduino Sketch
The arduino sketch here is an attempt to get the oled display and the sd card to work at the same time.

The official website can be found at [http://www.cosmicwatch.lns.mit.edu/](http://www.cosmicwatch.lns.mit.edu/)
The official github repository and resources can be found at [https://github.com/spenceraxani/CosmicWatch-Desktop-Muon-Detector-v2](https://github.com/spenceraxani/CosmicWatch-Desktop-Muon-Detector-v2)

Note that the current arduino code saves files in csv format. It should be possible to change this by commenting out the line "#define saveAsCSV".

I have not tested this with the python scripts, but they might work.

#Muon Detector GUI
This is a smallish program written in Unity using the C# programming language. It displays live information from the detector.
The compiled files can be found in the folder named build. Please note that I have not tested the Macintosh version and it may or may not work.