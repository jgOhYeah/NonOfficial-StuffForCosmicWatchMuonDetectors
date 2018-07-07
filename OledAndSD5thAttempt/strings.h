const char cosmicString[] PROGMEM = "Cosmic";
const char watchString[] PROGMEM = "Watch";
const char noSdCard[] PROGMEM = "No SD Card";
const char sdPresent[] PROGMEM = "SD Card Present";
#ifdef saveAsCSV
const char filename[] PROGMEM = "Muon_XYZ.csv";
  #define seperatorChar ','
#else
const char filename[] PROGMEM = "Muon_XYZ.txt";
  #define seperatorChar ' '
#endif
const char fileError[] PROGMEM = "File Error";
const char zero[] PROGMEM = "0";
const char muonString[] PROGMEM = " Muon Detector";
#define compiledPreprocessor "Compiled: " __DATE__ " at " __TIME__
const char compiledString[] PROGMEM = compiledPreprocessor;
#define firmwarePreprocessor "Firmware version: " firmwareVers
const char firmwareString[] PROGMEM = firmwarePreprocessor;
#define deviceIdPreprocessor "Device ID: " deviceId
const char deviceIdentity[] PROGMEM = deviceIdPreprocessor;
const char aboutString1[] PROGMEM = "Official website: http://cosmicwatch.lns.mit.edu/";
const char aboutString2[] PROGMEM = "This software can be found at: ";
const char aboutString3[] PROGMEM = "https://github.com/jgOhYeah/NonOfficial-StuffForCosmicWatchMuonDetectors";
const char csvHeader[] PROGMEM = "Count,Time Stamp,Measured ADC,SIPM Voltage,Dead Time,Temperature C";
const char loggingTo[] PROGMEM = "Logging to: ";
const char totalCount[] PROGMEM = "Count 0";
const char upTime[] PROGMEM = "Time";
//                                   0123456789ABCDEF - Position on display
const char sipmWow[] PROGMEM =       " |-Oh Yeah!!!-|";
const char detectorReady[] PROGMEM = " |---Ready----|";
const char rateString[] PROGMEM = "Rate";
const char masterString[] PROGMEM = "Master";
const char slaveString[] PROGMEM = "Slave";
const char colon[] PROGMEM = ":";



