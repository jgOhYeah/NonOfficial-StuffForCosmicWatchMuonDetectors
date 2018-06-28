const char cosmicString[] PROGMEM = "Cosmic";
const char watchString[] PROGMEM = "Watch";
const char noSdCard[] PROGMEM = "No SD Card";
const char sdPresent[] PROGMEM = "SD Card Present";
#ifdef saveAsCSV
const char comma[] PROGMEM = ",";
const char filename[] PROGMEM = "Muon_XYZ.csv";
#else
const char comma[] PROGMEM = " ";
const char filename[] PROGMEM = "MuonA_000.txt";
#endif
const char zero[] PROGMEM = "0";
const char csvHeader[] PROGMEM = "Count,Time Stamp,Measured ADC,SIPM Voltage,Dead Time,Temperature C";
const char loggingTo[] PROGMEM = "Logging to: ";
const char totalCount[] PROGMEM = "Total Count: ";
const char upTime[] PROGMEM = "Up Time: ";
const char sipmWow[] PROGMEM = "  ||--- Oh Yeah!!! ---||";
const char rateString[] PROGMEM = "Rate: ";
const char plusOrMinus[] PROGMEM = "+/-";
const char masterString[] PROGMEM = "Master";
const char slaveString[] PROGMEM = "Slave";
const char colon[] PROGMEM = ":";
