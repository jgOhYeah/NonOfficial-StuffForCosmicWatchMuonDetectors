/*Defines.h
 * Code adapted from Spencer Axiani's original software and designs by Jotham Gates
 * This file allows the software to be customised without having to scroll through many lines of code.
 */
#define FIRMWARE_VERSION "2.2"
//Pins
#define PIN_DETECTOR A0
#define PIN_TEMPERATURE A3
#define PIN_LED B00001000 //Bitmap of pin D3 - Does not use digitalWrite to save time and memory
#define PIN_SD_CARD 10
#define PIN_COINCIDENCE B01000000 //Bitmap of pin D6
#define OLED_RESET 10

//Interfaces to use. Comment out to disable non required ones. This will speed the detector up a little bit.
//If both serial and the sd card are not required, the arduino will not bother to take readings from the temperature sensor or format the values as a string.
//If the screen is used, it will be updated once per second.
#define USE_SCREEN //Whether to include the library and code for driving the oled display or not
#define USE_SD_CARD //Whether to include the library and code for writing to the sd card
#define USE_SERIAL //Whether to enable serial
#define USE_SERIAL_SETTINGS //Whether to include code for saving the detector name into eeprom without having to flash naming.ino
                          //Will only be enabled if USE_SERIAL is enabled as well

//Thresholds
#define THRESHOLD_SIGNAL 50
#define THRESHOLD_RESET 15
#define THRESHOLD_LARGE 180 //Originally 180, changed to 140 to match up with space on screen

//Calibration
const long double cal[] = {-9.085681659276021e-27, 4.6790804314609205e-23, -1.0317125207013292e-19,
  1.2741066484319192e-16, -9.684460759517656e-14, 4.6937937442284284e-11, -1.4553498837275352e-08,
   2.8216624998078298e-06, -0.000323032620672037, 0.019538631135788468, -0.3774384056850066, 12.324891083404246};

//General
#define NUMBER_FILES_TO_DELETE 5 //If full, start at the beginning and delete 5 more files ahead
#define FILENAME_NUMBER_START_POS 5 //Position of the hundreds char in the filename
#define MAX_FILES 200 //Maximum file number before rolls back to 0 Needs to be less then 255 - NUMBER_FILES_TO_DELETE
#define SCREEN_UPDATE_INTERVAL 1000
#define TEMPERATURE_SENSOR_OFFSET 500 //The voltage (in mV) produced by the temperature sensor when the temperature is 0.
                            //TMP36 (the one that is specified in the design): 500
                            //TMP35: 0
#define SERIAL_TIMEOUT 10000 //Timeout for serial reads

//EEPROM Settings addresses
//Make sure that these are the same as the naming sketch - needs to be the version modified for this arduino code
//This is the address in the EEPROM of the first / only byte that a setting is stored.
#define EEPROM_ID1 0
#define EEPROM_ID2 40 //By using 40, first line should be the normal naming.ino setting
#define EEPROM_FILE_FORMAT 80
#define EEPROM_STRING_LENGTH 40
#define EEPROM_CONSTRAST 81
