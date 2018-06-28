//Pins
#define detectorPin A0
#define temperaturePin A3
#define ledPin 3
#define sdCardPin 10
#define coincidencePin 6
#define OLED_RESET 10

//Thresholds
#define signalThreshold 50
#define resetThreshold 15
#define largePulseThreshold 180

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
