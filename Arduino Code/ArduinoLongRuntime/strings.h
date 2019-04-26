/*Strings.h
 * Code adapted from Spencer Axiani's original software and designs by Jotham Gates
 * This is a collection of nearly all the strings used in this program so it is easyish to change the wording
 * Note that if you change some of the strings on this page, you might have to fiddle around a bit with the coordinates in
 * "u8x8.draw1x2String(x coordinate (0 to 15),y coordinate (0 to 7),text);"
 * at various parts of this program if the spacing goes weird.
 * Note also that the u8x8 library wraps text around to the start of the same line if it overflows.
 */
const char cosmicString[] PROGMEM = "Cosmic";
const char watchString[] PROGMEM = "Watch";
const char noSdCard[] PROGMEM = "No SD Card";
const char sdPresent[] PROGMEM = "SD Card Present";
const char filename[] PROGMEM = "Muon_";
const char fileError[] PROGMEM = "File Error";
const char zero[] PROGMEM = "0";
const char muonString[] PROGMEM = " Muon Detector";
#define compiledPreprocessor "Compiled: " __DATE__ " at " __TIME__
const char compiledString[] PROGMEM = compiledPreprocessor;
#define firmwarePreprocessor "Firmware version: " firmwareVers
const char firmwareString[] PROGMEM = firmwarePreprocessor;
const char deviceIdentity[] PROGMEM = "Device ID: ";
const char aboutString1[] PROGMEM = "Official website: http://cosmicwatch.lns.mit.edu/";
const char aboutString2[] PROGMEM = "This software can be found at: ";
const char aboutString3[] PROGMEM = "https://github.com/jgOhYeah/NonOfficial-StuffForCosmicWatchMuonDetectors";
const char csvHeader[] PROGMEM = "Count,Time Stamp,Measured ADC,SIPM Voltage,Dead Time,Temperature C";
const char txtHeader[] PROGMEM = "Count Time_Stamp Measured_ADC SIPM_Voltage Dead_Time Temperature_C";
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
const char settingsInstructions[] PROGMEM = "To change various settings, type \"Settings\"";
const char settingsInstructions2[] PROGMEM = " before the detector starts detecting";
const char settingsHeading[] PROGMEM = "Settings Entered.";
const char settingsHeading1[] PROGMEM = "\t- Set line one of the detector ID: Type \"ID1 \" followed by the name";
const char settingsHeading2[] PROGMEM = "\t- Set line two of the detector ID: Type \"ID2 \" followed by the name";
const char settingsHeading3[] PROGMEM = "\t- Set the file format saved (txt or csv):";
const char settingsHeading4[] PROGMEM =  " Type \"Format \" followed by \".txt\" or \".csv\"";
const char settingsHeading5[] PROGMEM = "\t- Set display contrast: Type \"Contrast \" followed by 0 (dim) to 255 (bright)";
const char settingsAndNL[] PROGMEM = " and then a new line.";
const char experiencedSuccess[] PROGMEM = "Success";
const char noSuccess[] PROGMEM = "Error. Please try again";
const char resetToExit[] PROGMEM = "Reset to exit";
const char mistakeText[] PROGMEM = "If you make a mistake, please type \"x x \" to continue";
const char settingsString[] PROGMEM = "Settings";
const char id1String[] PROGMEM = "ID1";
const char id2String[] PROGMEM = "ID2";
const char formatString[] PROGMEM = "Format";
const char contrastString[] PROGMEM = "Contrast";
const char txtString[] PROGMEM = ".txt";
const char csvString[] PROGMEM = ".csv";
#define barChartChar '-'
#define slaveChar 'S'
#define masterChar 'M'
