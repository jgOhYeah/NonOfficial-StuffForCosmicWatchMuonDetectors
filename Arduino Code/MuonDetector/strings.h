/*Strings.h
 * Code adapted from Spencer Axiani's original software and designs by Jotham Gates
 * This is a collection of nearly all the strings used in this program so it is easyish to change the wording
 * Note that if you change some of the strings on this page, you might have to fiddle around a bit with the coordinates in
 * "u8x8.draw1x2String(x coordinate (0 to 15),y coordinate (0 to 7),text);"
 * at various parts of this program if the spacing goes weird.
 * Note also that the u8x8 library wraps text around to the start of the same line if it overflows.
 * 
 * The bar graph is made with 2 bitmap images. It is 16px high and 120px accross. Each byte represents an 8px high column, with bit 0 being the top row
 * and bit 7 the bottom. The bottom half (8px by 120px) is the scale label and numbers and is held in one array stored in PROGMEM. This is written to
 * the display when the detector first starts and is never overwritted. The top half is also stored in an array.
 * When the display needs to be updated, it is copied into RAM and the bar representing the strength of the recieved signal is overlayed on top
 * by exclusive oring CHAR_BAR_GRAPH with the required number of columns. This inverts areas of the display, allowing the bar to be seen. The display is inverted
 * instead of just being made white so that the scale can be still seen.
 * 
 * To generate the bitmap, the image was drawn using GIMP, and PNG images of the required segments exported. These were uploaded to https://javl.github.io/image2cpp/,
 * with the settings of the background colour being black, the output in plain bytes and the draw mode as vertical.
 * 
 * Pixel Layout for an 8*8 block on the SSD1306 display
 * Byte (Column)  0 1 2 3 4 5 6 7 ... keeps going on
 * Rows           0 0 0 0 0 0 0 0
 *                1 1 1 1 1 1 1 1
 *                2 2 2 2 2 2 2 2
 *                3 3 3 3 3 3 3 3
 *                4 4 4 4 4 4 4 4
 *                5 5 5 5 5 5 5 5
 *                6 6 6 6 6 6 6 6
 *                7 7 7 7 7 7 7 7
 */
const char cosmicString[] PROGMEM = "Cosmic";
const char watchString[] PROGMEM = "Watch";
const char noSdCard[] PROGMEM = "No SD Card";
const char sdPresent[] PROGMEM = "SD Card Present";
const char filename[] PROGMEM = "MUON_";
const char fileError[] PROGMEM = "File Error";
const char zero[] PROGMEM = "0";
const char muonString[] PROGMEM = " Muon Detector";
#define PREPROCESSOR_COMPILED "Compiled: " __DATE__ " at " __TIME__
const char compiledString[] PROGMEM = PREPROCESSOR_COMPILED;
#define PREPROCESSOR_FIRMWARE "Firmware version: " FIRMWARE_VERSION
const char firmwareString[] PROGMEM = PREPROCESSOR_FIRMWARE;
const char deviceIdentity[] PROGMEM = "Device ID: ";
const char aboutString1[] PROGMEM = "Official website: http://cosmicwatch.lns.mit.edu/";
const char aboutString2[] PROGMEM = "This firmware can be found at: ";
const char aboutString3[] PROGMEM = "https://github.com/jgOhYeah/NonOfficial-StuffForCosmicWatchMuonDetectors"; //Copied to ram in 2 parts to save ram. First 50 chars first then the rest. Must be less than the char buffer size + 50-1
const char csvHeader[] PROGMEM = "Count,Time Stamp ms,Measured ADC 0-1023,SIPM Voltage mV,Dead Time ms,Temperature C"; //Copied to ram in 2 parts to save ram. First 50 chars first then the rest
const char txtHeader[] PROGMEM = "Count Time_Stamp_ms Measured_ADC_0-1023 SIPM_Voltage_mV Dead_Time_ms Temperature_C"; //Copied to ram in 2 parts to save ram. First 50 chars first then the rest
const char loggingTo[] PROGMEM = "Logging to: ";
const char totalCount[] PROGMEM = "Count 0";
const char upTime[] PROGMEM = "Time";
//                                   0123456789ABCDEF - Position on display
const char sipmWow[] PROGMEM =       "   Oh Yeah!!!  ";
const char detectorReady[] PROGMEM = "     Ready     ";
const char rateString[] PROGMEM = "Rate";
const char masterString[] PROGMEM = "Master";
const char slaveString[] PROGMEM = "Slave";
const char colon[] PROGMEM = ":";
const char settingsInstructions[] PROGMEM = "To change various settings, type \"Settings\"";
const char settingsInstructions2[] PROGMEM = " Before the detector starts detecting";
const char settingsHeading[] PROGMEM = "Settings Entered.";
const char settingsHeading1[] PROGMEM = "\t- Set line one of the detector ID: Type \"ID1 \" followed by the name";
const char settingsHeading2[] PROGMEM = "\t- Set line two of the detector ID: Type \"ID2 \" followed by the name";
const char settingsHeading3[] PROGMEM = "\t- Set the file format saved (TXT or CSV):";
const char settingsHeading4[] PROGMEM =  " Type \"Format \" followed by \".TXT\" or \".CSV\"";
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
const char txtString[] PROGMEM = ".TXT";
const char csvString[] PROGMEM = ".CSV";
#define CHAR_BAR_CHART '-'
#define CHAR_SLAVE 'S'
#define CHAR_MASTER 'M'
#define CHAR_BAR_GRAPH 0x7f //B01111111
//Bitmap of the bottom scale of the bar graph (8*120)
const byte barScaleB[] PROGMEM ={
  0xff, 0xff, 0x00, 0x3c, 0x62, 0x52, 0x4a, 0x46, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x24, 0x4a, 0x4a, 0x4a, 0x30, 0x00,
  0x74, 0x00, 0x7e, 0x12, 0x12, 0x12, 0x0c, 0x00, 0x7e, 0x04, 0x08, 0x04, 0x7e, 0x00, 0x00, 0x00,
  0x00, 0x7e, 0x12, 0x12, 0x12, 0x0c, 0x00, 0x38, 0x54, 0x54, 0x54, 0x48, 0x40, 0x00, 0x20, 0x54,
  0x54, 0x54, 0x78, 0x00, 0x7e, 0x10, 0x10, 0x28, 0x44, 0x00, 0x00, 0x00, 0x00, 0x78, 0x08, 0x70,
  0x08, 0x70, 0x00, 0x1e, 0x20, 0x40, 0x40, 0x20, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x40, 0x44, 0x7e, 0x40, 0x40, 0x00, 0x34, 0x4a, 0x4a, 0x4a, 0x34, 0x00, 0x3c,
  0x62, 0x52, 0x4a, 0x46, 0x3c, 0x00, 0xff, 0xff};
const byte barScaleT[] PROGMEM ={
  0xff, 0xff, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xf8, 0x80,
  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xff, 0x80, 0x80, 0x80, 0x80,
  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xf8, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
  0x80, 0x80, 0x80, 0x80, 0x80, 0xff, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
  0x80, 0x80, 0xf8, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xff,
  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xf8, 0x80, 0x80, 0x80,
  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xff, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
  0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xff, 0xff};

//Bitmap of a colon and space together (2 8x8)
const byte offsetColon1[] PROGMEM = {B00110000,0,0,0,0,0,0,0}; //Char that puts a colon on the left side to make it appear as a colon and a space
const byte offsetColon2[] PROGMEM = {B00001100,0,0,0,0,0,0,0}; //Char that puts a colon on the left side to make it appear as a colon and a space

//Custom Characters with bits of the bar graph for the master and slave letters.
/* Not bothering with that anymore but leaving in just in case
const byte charMasterTop[] PROGMEM = {0xfc, 0x30, 0xc0, 0xc0, 0x30, 0xfc, 0x00, 0xff};
const byte charMasterBottom[] PROGMEM = {0x3f, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x00, 0xff};
const byte charSlaveTop[] PROGMEM = {0x30, 0xcc, 0xcc, 0xcc, 0xcc, 0x00, 0x00, 0xff};
const byte charSlaveBottom[] PROGMEM = {0x0c, 0x30, 0x30, 0x30, 0x30, 0x0f, 0x00, 0xff};
*/
