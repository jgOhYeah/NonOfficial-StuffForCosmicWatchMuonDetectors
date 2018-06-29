# NonOfficial-StuffForCosmicWatchMuonDetectors
Extra code and stuff I have made/adapted for Spencer Axiani's Cosmic Watch Desktop Muon Detector V2
This is a work in progress and there might be the odd bug and issue.

The arduino sketch here is an attempt to get the oled display and the sd card to work at the same time.

The official website can be found at [http://www.cosmicwatch.lns.mit.edu/](http://www.cosmicwatch.lns.mit.edu/)
The official github repository and resources can be found at [https://github.com/spenceraxani/CosmicWatch-Desktop-Muon-Detector-v2](https://github.com/spenceraxani/CosmicWatch-Desktop-Muon-Detector-v2)

Note that the current arduino code saves files in csv format and ues commas instead of spaces when sending serial data. It is possible to change this by commenting out the line "#define saveAsCSV". Failing that, change the const char comma variable in strings.h to be a space.

Also note that this version does not fully support the python scripts and naming detectors yet.
