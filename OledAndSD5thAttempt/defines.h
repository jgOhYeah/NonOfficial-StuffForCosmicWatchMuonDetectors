#define firmwareVers "2.0 non official"
#define deviceId "Muon Detector A"
//Pins
#define detectorPin A0
#define temperaturePin A3
#define ledPin B00001000 //Bitmap of pin D3 - Does not use digitalWrite to save time and memory
#define sdCardPin 10
#define coincidencePin B01000000 //Bitmap of pin D6
#define OLED_RESET 10

//Interfaces to use. Comment out to disable non required ones. This will speed the detector up a little bit.
//If both serial and the sd card are not required, the arduino will not bother to take readings from the tmperature sensor or format the values as a string.
//If the screen is used, it will be updated once per second.
#define useScreen //Whether to include the library and code for driving the oled display or not
#define useSdCard //Whether to include the library and code for writing to the sd card
#define useSerial //Whether to enable serial

//Thresholds
#define signalThreshold 50
#define resetThreshold 15
#define largePulseThreshold 180 //Originally 180, changed to 140 to match up with space on screen

//Calibration
const long double cal[] = {-9.085681659276021e-27, 4.6790804314609205e-23, -1.0317125207013292e-19,
  1.2741066484319192e-16, -9.684460759517656e-14, 4.6937937442284284e-11, -1.4553498837275352e-08,
   2.8216624998078298e-06, -0.000323032620672037, 0.019538631135788468, -0.3774384056850066, 12.324891083404246};

//General
#define numberOfFilesToDelete 5 //If full, start at the beginning and delete 5 more files ahead
#define fileNumberStartPoint 5 //Position of the hundreds char in the filename
#define maxNumberOfFiles 200 //Maximum file number before rolls back to 0 Needs to be less then 255 - numberOfFilesToDelete
#define screenUpdateTime 1000
#define saveAsCSV //If left in, saves .csv files. If commented out, saves .txt files.
//This also affects the serial port - use txt with the python scripts for uploading to a webserver.
#define barChartChar '-'
#define slaveChar 'S'
#define masterChar 'M'
#define tempSenseOffset 500 //The voltage (in mV) produced by the temperature sensor when the temperature is 0.
                            //TMP36 (the one that is specified in the design): 500
                            //TMP35: 0
