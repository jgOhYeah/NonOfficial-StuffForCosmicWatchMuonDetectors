/*
Cosmic Watch Muon Detector alternative software
   Non Official Version of the CosmicWatch Desktop Muon Detector Arduino Code
   Code adapted from Spencer Axiani's original software and designs by Jotham Gates
   The original code and documentation / designs can be found here:
   https://github.com/spenceraxani/CosmicWatch-Desktop-Muon-Detector-v2

   This software uses both the u8x8 display and the sd card if they are fitted.
   Differences to the original code are mainly:
    - Using a smaller library for the driving the display to save RAM and is also fairly fast.
    - Able to save files both as csv and the original text file - selectable in the settings menu
      - CSV does not have to be imported into spreadsheet applications, text does (at least for me)
      - Text is what the original detector software saves files as, for compatability - note that the header is slightly different and might cause the odd issue, but the data is structured the same.
    - Tries to use char arrays instead of the String library to save RAM.
    - Does not fully support the python scripts for downloading and deleting files from the sd card
    - EEPROM can be programmed by the main program - type "Settings" (with the serial terminal set to \n as new line) before the detector finishes starting up.
    - The screen has a day counter as well as hours, minutes and seconds
    - Graphical bar graph with something of a scale instead of one made from hyphens.
    
   Libraries required:
   Builtin:
    Wire
    SD
    EEPROM
   Need to be installed:
    U8g2 - u8x8 text only part required
    This can be found by searching for it in the libraries manager (sketch > include library > manage libraries)

   Files:
    This file contains the main code.
    The defines.h file contains parameters that can be adjusted to adapt this to your needs and preferences.
    The strings.h file contains most of the strings in this program, so change these if you want to change the wording.
    The oled.ino file contains functions relating to driving the display.
    The sdCard.ino file contains functions related to the sdCard.
    The serial.ino file contains functions relating to serial and built in EEPROM settings function.
    The other.ino file contains other general functions.
   
   WARNING ABOUT WORKING ON THIS PROGRAM:
   An atmega328p, as found in the arduino Uno and Nanos has only 2048 bytes of RAM.
   The SD card library is very big, and combined with driving the display, there are only a few hundred bytes
   of ram left for local variables. Possibly less than 100 at various times. If you are trying to edit this
   and are getting really weird results and behaviour in other areas of code / restarts. Running out of ram is likely to be the issue.
*/
//Include the defines.h file in the same directory
#include "defines.h"
#include "strings.h"

#ifdef USE_SCREEN
//LCD Libraries. This one is slightly smaller than the adafruit one, although with a few less features.
#include <Wire.h>
#include <U8x8lib.h>
#endif
//SD card
#ifdef USE_SD_CARD
#include <SD.h>
File myFile;
#endif
#ifdef USE_SCREEN
U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(U8X8_PIN_NONE);
#endif
#include <EEPROM.h>
bool isMaster = true;
unsigned long count = 0;
float sipmVoltage = 0;

//Timing
unsigned int startTime = 0;
unsigned long totalDeadtime = 0;
#ifdef USE_SD_CARD
//SD Card stuff
bool isSDCard = false;
char separatorChar = ' ';
#endif

#ifdef USE_SERIAL_DEBUG //Automatically add and remove the print ram left as required
  #define DEBUG_RAM() debugRam(__LINE__)
#else
  #define DEBUG_RAM()
#endif

void setup() {
  //Set up ADC
  analogReference (EXTERNAL); //DO NOT REMOVE THIS LINE!!!!!!!!!!!!!!
  //It will probably blow up the microcontroller if an analogRead() is called before the reference is set properly.
  //The internal reference and external 3.3v supply will be shorted otherwise.
  ADCSRA &= ~(bit (ADPS0) | bit (ADPS1) | bit (ADPS2));  // clear prescaler bits
  ADCSRA |= bit (ADPS0) | bit (ADPS1);                   // Set prescaler to 8
  //Make the led pin an output
  //pinMode(PIN_LED, OUTPUT);
  //pinMode(PIN_COINCIDENCE, INPUT);
  DDRD = (DDRD | PIN_LED) & (~PIN_COINCIDENCE); //Set the led to be an output and the coincidence pin to be an input
  if ((PIND & PIN_COINCIDENCE) == PIN_COINCIDENCE) { //If the coincidence pin is high, then put into slave mode
    isMaster = false;
    PORTD = PORTD | PIN_LED; //Turn the led on
  } else {
    //pinMode(PIN_COINCIDENCE, OUTPUT);
    //digitalWrite(PIN_COINCIDENCE,HIGH);
    DDRD = DDRD | PIN_COINCIDENCE; //Make the coincidence pin an output
    PORTD = PORTD | PIN_COINCIDENCE; //Turn the coincidence pin high
  }
#ifdef USE_SCREEN
  //Start the display
  u8x8.begin();
  u8x8.setFlipMode(1);
  u8x8.setPowerSave(0);
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.setContrast(EEPROM.read(EEPROM_CONSTRAST));
#endif
  //Put screen upside down
#ifdef USE_SERIAL
  //Set up serial
  Serial.begin(9600);
  Serial.setTimeout(3000);
#endif
  DEBUG_RAM();
  //Ram is limited, so only copy the big strings in parts
  char charBuffer[60] = ""; //Define as an empty string to keep the compiler happy. Make it 122 bytes so that it can also be used for the screen
  DEBUG_RAM();
#if defined USE_SCREEN || defined USE_SERIAL
  //Strings stored in PROGMEM need to be copied to RAM before they can be used. charBuffer is a temporary spot for them.
  strcpy_P(charBuffer, cosmicString);
#ifdef USE_SCREEN
  u8x8.draw2x2String(0, 0, charBuffer);
#endif
#ifdef USE_SERIAL
  Serial.write(charBuffer);
  Serial.write(' ');
  DEBUG_RAM();
#endif
  strcpy_P(charBuffer, watchString);
#ifdef USE_SERIAL
  Serial.write(charBuffer);
#endif
#ifdef USE_SCREEN
  u8x8.draw2x2String(6, 2, charBuffer);
  getEepromString(EEPROM_ID1,16,charBuffer);
  u8x8.draw1x2String(0,4,charBuffer);
  byte secondRowLength = getEepromString(EEPROM_ID2,16,charBuffer);
  u8x8.draw1x2String(17 - secondRowLength,6,charBuffer);
#endif
#ifdef USE_SERIAL
  strcpy_P(charBuffer, muonString);
  Serial.println(charBuffer);
  strcpy_P(charBuffer, deviceIdentity);
  Serial.write(charBuffer);
  getEepromString(EEPROM_ID1,EEPROM_STRING_LENGTH,charBuffer);
  Serial.write(charBuffer);
  getEepromString(EEPROM_ID2,EEPROM_STRING_LENGTH,charBuffer);
  Serial.println(charBuffer);
  strcpy_P(charBuffer, firmwareString);
  Serial.println(charBuffer);
  strcpy_P(charBuffer, compiledString);
  Serial.println(charBuffer);
  //About string 1
  strcpy_P(charBuffer, aboutString1);
  Serial.println(charBuffer);
  strcpy_P(charBuffer, aboutString2);
  Serial.write(charBuffer);
  //About string 3
  strncpy_P(charBuffer, aboutString3,50); //Do it in two parts to save ram
  Serial.write(charBuffer);
  strcpy_P(charBuffer, aboutString3+50); //Do it in two parts
  Serial.println(charBuffer);
  
  //strcpy_P(charBuffer, aboutString3);
  //Serial.println(charBuffer);
#ifdef USE_SERIAL_SETTINGS //Print out the instructions for entering settings mode if it is included
  strcpy_P(charBuffer, settingsInstructions);
  Serial.write(charBuffer);
  strcpy_P(charBuffer, settingsAndNL);
  Serial.write(charBuffer);
  strcpy_P(charBuffer, settingsInstructions2);
  Serial.println(charBuffer);
  DEBUG_RAM();
#endif
  Serial.println();
#endif
#endif
  delay(1000);
#ifdef USE_SD_CARD
  //Set up the sd card
  if (SD.begin(PIN_SD_CARD)) {
#if defined USE_SCREEN || defined USE_SERIAL
    strcpy_P(charBuffer, sdPresent);
#ifdef USE_SERIAL
    Serial.println(charBuffer);
#endif
    //Put here as sd.begin has a time delay
    u8x8.clear();
#ifdef USE_SCREEN
    u8x8.draw1x2String(0, 0, charBuffer);
#endif
    strcpy_P(charBuffer, loggingTo);
#ifdef USE_SCREEN
    u8x8.draw1x2String(0, 2, charBuffer);
#endif
#ifdef USE_SERIAL
    Serial.print(charBuffer);
#endif
    DEBUG_RAM();
#endif
    isSDCard = true;
    //Now go through the process of trying to find an empty file name to save
    openLogFile(charBuffer);
    DEBUG_RAM();
#ifdef USE_SERIAL
    Serial.println(charBuffer); //Print the file name logging to
#endif
#ifdef USE_SCREEN
    u8x8.draw1x2String(0, 4, charBuffer); //Display the file name
#endif
  } else {
    strcpy_P(charBuffer, noSdCard);
#ifdef USE_SERIAL
    Serial.println(charBuffer);
#endif
    //Put here as sd.begin has a time delay
#ifdef USE_SCREEN
    u8x8.clear();
    u8x8.draw1x2String(0, 0, charBuffer);
#endif
  }
#ifdef USE_SD_CARD
  DEBUG_RAM();
  if (isSDCard) { //Write the headers to the sd card file
    strcpy_P(charBuffer, cosmicString);
    myFile.write(charBuffer);
    myFile.write(' ');
    strcpy_P(charBuffer, watchString);
    myFile.write(charBuffer);
    strcpy_P(charBuffer, muonString);
    myFile.println(charBuffer);
    strcpy_P(charBuffer, deviceIdentity);
    myFile.write(charBuffer);
    getEepromString(EEPROM_ID1,EEPROM_STRING_LENGTH,charBuffer);
    myFile.write(charBuffer);
    getEepromString(EEPROM_ID2,EEPROM_STRING_LENGTH,charBuffer);
    myFile.println(charBuffer);
    strcpy_P(charBuffer, firmwareString);
    myFile.println(charBuffer);
    strcpy_P(charBuffer, compiledString);
    myFile.println(charBuffer);
    strcpy_P(charBuffer, aboutString1);
    myFile.println(charBuffer);
    strcpy_P(charBuffer, aboutString2);
    myFile.write(charBuffer);
    //About string 3
    strncpy_P(charBuffer, aboutString3,50); //Do it in two parts to save ram
    myFile.write(charBuffer);
    strcpy_P(charBuffer, aboutString3+50); //Do it in two parts
    myFile.println(charBuffer);
    //strcpy_P(charBuffer, aboutString3);
    //myFile.println(charBuffer);
    myFile.println();
    //Will flush the file in a little bit.
  }
#endif
#endif
#if defined USE_SCREEN || defined USE_SERIAL || defined USE_SD_CARD
  if (isMaster) {
    strcpy_P(charBuffer, masterString);
  } else {
    strcpy_P(charBuffer, slaveString);
  }
#endif
#ifdef USE_SCREEN
  u8x8.draw1x2String(0, 6, charBuffer);
#endif
#ifdef USE_SD_CARD
  if (isSDCard) {
    myFile.println(charBuffer);
    myFile.flush();
  }
#endif
#ifdef USE_SERIAL
  Serial.println(charBuffer);
#endif
  DEBUG_RAM();
  delay(1000);
#ifdef USE_SERIAL
  //TXT Header only for serial
  strncpy_P(charBuffer, txtHeader,50); //Do it in two parts to save ram
  Serial.write(charBuffer);
  strcpy_P(charBuffer, txtHeader+50); //Do it in two parts
  Serial.println(charBuffer);
#endif
#ifdef USE_SD_CARD
  if (isSDCard) {
    //Get the header with commas instead of spaces if saving as a csv
    if(separatorChar == ',') {
      strcpy_P(charBuffer, csvHeader);
      //CSV Header only for serial
      strncpy_P(charBuffer, csvHeader,50); //Do it in two parts to save ram
      myFile.write(charBuffer);
      strcpy_P(charBuffer, csvHeader+50); //Do it in two parts
      myFile.println(charBuffer);
    } else {
      //TXT Header only for serial
      strncpy_P(charBuffer, txtHeader,50); //Do it in two parts to save ram
      myFile.write(charBuffer);
      strcpy_P(charBuffer, txtHeader+50); //Do it in two parts
      myFile.println(charBuffer);
    }
    myFile.flush();
  }
  DEBUG_RAM();
#endif
  if (isMaster) {
    //digitalWrite(PIN_COINCIDENCE,LOW);
    PORTD = PORTD & (~PIN_COINCIDENCE); //Make the coincidence pin low
  }
  PORTD = PORTD & (~PIN_LED); //Make the led pin low
#ifdef USE_SCREEN
  //Set up the display for normal viewing
  //const byte offsetColon1[] = {B00110000,0,0,0,0,0,0,0}; //Char that puts a colon on the left side to make it appear as a colon and a space
  //const byte offsetColon2[] = {B00001100,0,0,0,0,0,0,0}; //Char that puts a colon on the left side to make it appear as a colon and a space
  u8x8.clear();
  strcpy_P(charBuffer, totalCount);
  u8x8.draw1x2String(0, 0, charBuffer);
  strcpy_P(charBuffer, upTime);
  u8x8.draw1x2String(0, 2, charBuffer);
  //byte tempTile[120]; //A separate buffer runs out of ram with sd card, so have to use charBuffer as a buffer of bytes, not chars.
  memcpy_P(charBuffer,offsetColon1,8);
  u8x8.drawTile(5,0,1,charBuffer);
  u8x8.drawTile(4,2,1,charBuffer);
  u8x8.drawTile(4,6,1,charBuffer);
  memcpy_P(charBuffer,offsetColon2,8);
  u8x8.drawTile(5,1,1,charBuffer);
  u8x8.drawTile(4,3,1,charBuffer);
  u8x8.drawTile(4,7,1,charBuffer);
  if (isMaster) {
    /*memcpy_P(tempTile,charMasterTop,8);
    u8x8.drawTile(0,4,1,tempTile);
    memcpy_P(tempTile,charMasterBottom,8);
    u8x8.drawTile(0,5,1,tempTile);*/
    u8x8.draw1x2Glyph(0, 4, CHAR_MASTER);
  } else {
    /*memcpy_P(tempTile,charSlaveTop,8);
    u8x8.drawTile(0,4,1,tempTile);
    memcpy_P(tempTile,charSlaveBottom,8);
    u8x8.drawTile(0,5,1,tempTile);*/
    u8x8.draw1x2Glyph(0, 4, CHAR_SLAVE);
  }
  strcpy_P(charBuffer, rateString);
  u8x8.draw1x2String(0, 6, charBuffer);
  for(byte i = 0;i<15;i++) { //We do not have enough ram to do this all at once, so so each block at a time
    memcpy_P(charBuffer,barScaleB+i*8,8); //i<<3 should be i*8
    u8x8.drawTile(i+1,5,1,charBuffer);
  }
  DEBUG_RAM();
#endif
#if defined USE_SERIAL && defined USE_SERIAL_SETTINGS
  enterSettings();
#endif
  DEBUG_RAM();
  analogRead(PIN_DETECTOR);
  startTime = millis();
}

void loop() {
  if (analogRead(PIN_DETECTOR) > THRESHOLD_SIGNAL) {
    int detectionADC = analogRead(PIN_DETECTOR);
    unsigned long timeStamp = micros();
    unsigned long timeDetecting = millis() - startTime;
    bool usePulse = false;
    //digitalWrite(PIN_LED,HIGH);
    //We detected something!
    if (isMaster) {
      PORTD = PORTD | PIN_COINCIDENCE; //Make the led pin high
      usePulse = true;
    }
#if defined USE_SCREEN || defined USE_SERIAL
    //Swap the adc to the temperature pin and measure a few times to stabilise
    //Also delay a while to allow the coincidence pin to stay high and allow master detector to trigger if slave.
    /*Serial.println("About to read temp");*/
    analogRead(PIN_TEMPERATURE); //Swap the adc to the temperature pin and throw away the first measurement as it might be off by a bit
    float temperatureC = (((analogRead(PIN_TEMPERATURE) + analogRead(PIN_TEMPERATURE)) * (1650. / 1024)) - TEMPERATURE_SENSOR_OFFSET) / 10. ;
    //adc * 3300 - 1024 * TEMPERATURE_SENSOR_OFFSET / 10
    /*Serial.print("Temp is: ");
      Serial.println(temperatureC);*/
    //Make the text for the file and serial without using the String library to save RAM
#endif
    if (!isMaster) {
      if ((PIND & PIN_COINCIDENCE) == PIN_COINCIDENCE) {
        usePulse = true;
      }
    }
    if (usePulse) {
      /*Serial.println("Using Pulse");*/
      count++;
      //digitalWrite(PIN_LED,HIGH);
      PORTD = PORTD | PIN_LED; //Make the led pin high
#if defined USE_SCREEN || defined USE_SERIAL
      //In the form of:
      //count,timeStamp,detectionADC,SIPM Voltage,deadTime,temperature
      sipmVoltage = get_sipm_voltage(detectionADC);
      /*Serial.print("SiPM Voltage is: ");
        Serial.println(sipmVoltage);*/
      char countString[11];
      char timeString[11];
      char adcString[5];
      char sipmVString[7];
      char deadtimeString[11];
      char tempCString[7];
      ultoa(count, countString, 10);
      ultoa(timeDetecting, timeString, 10);
      itoa(detectionADC, adcString, 10);
      byte decimalPlaces = 1;
      if (sipmVoltage >= 100 || sipmVoltage < 0) {
        decimalPlaces = 0;
      }
      dtostrf(sipmVoltage, 1, decimalPlaces, sipmVString);
      ultoa(totalDeadtime, deadtimeString, 10);
      dtostrf(temperatureC, 1, 1, tempCString);
#ifdef USE_SERIAL
      //Always use spaces as a seperator so is compatible with the python scripts
      //The real time graphing in the unity program accepts either commas or spaces
      Serial.write(countString);
      Serial.write(' ');
      Serial.write(timeString);
      Serial.write(' ');
      Serial.write(adcString);
      Serial.write(' ');
      Serial.write(sipmVString);
      Serial.write(' ');
      Serial.write(deadtimeString);
      Serial.write(' ');
      Serial.write(tempCString);
#ifdef USE_SERIAL_DEBUG //Add the number of bytes left in RAM to the end.
      Serial.write(' ');
      int ramLeft = freeRam(); //Only have to calculate once if both SD card and Serial enabled.
      Serial.print(ramLeft);
#endif
      Serial.println();
#endif
#ifdef USE_SD_CARD
      if (isSDCard) {
        myFile.write(countString);
        myFile.write(separatorChar);
        myFile.write(timeString);
        myFile.write(separatorChar);
        myFile.write(adcString);
        myFile.write(separatorChar);
        myFile.write(sipmVString);
        myFile.write(separatorChar);
        myFile.write(deadtimeString);
        myFile.write(separatorChar);
        myFile.write(tempCString);
#ifdef USE_SERIAL_DEBUG //Add the number of bytes left in RAM to the end.
#ifndef USE_SERIAL
        int ramLeft = freeRam(); //Only have to calculate once if both SD card and Serial enabled.
#endif
        myFile.write(separatorChar);
        myFile.print(ramLeft);
#endif
        myFile.println();
#ifdef SD_CARD_SKIP_WRITES //Only write to the sd card every few detections
        //unsigned long sdTime = micros();
        if((count & 0x01) == 0x00) { //Every second count - bit 0 == 0
          //Serial.println(F("Writing"));
          myFile.flush();
        } //else {
          //Serial.println(F("Skip"));
        //}
        //Serial.print(F("SD time: "));
        //Serial.println(micros()-sdTime);
#else
        myFile.flush();
#endif
      }
#endif
#endif
    }
    //digitalWrite(PIN_LED,LOW);
    PORTD = PORTD & (~PIN_LED); //Make the led pin low
    //Wait until the pulse falls back so does not trigger multiple times for the one muon.
    while (analogRead(PIN_DETECTOR) > THRESHOLD_RESET) {
    }

    if (isMaster) {
      //digitalWrite(PIN_COINCIDENCE,LOW);
      PORTD = PORTD & (~PIN_COINCIDENCE); //Make the coincidence pin low
    }
    totalDeadtime += (micros() - timeStamp) / 1000;
  }
#ifdef USE_SCREEN
  updateTime();
#endif
}
