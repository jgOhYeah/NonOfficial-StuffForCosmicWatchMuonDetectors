/*
   Non Official Version of the CosmicWatch Desktop Muon Detector Arduino Code
   Code adapted from Spencer Axiani's original software and designs by Jotham Gates
   The original code and documentation / designs can be found here:
   https://github.com/spenceraxani/CosmicWatch-Desktop-Muon-Detector-v2

   This software uses both the u8x8 display and the sd card if they are fitted.
   Differences to the original version are mainly:
    - Using a smaller library for the driving the display to save RAM it is also fairly fast
    - Able to save files both as csv and the original text file - selectable in the settings menu
      - CSV does not have to be imported into spreadsheet applications, text does (at least for me)
      - Text is what the original detector software saves files as, for compatability - note that the header is slightly different and might cause the odd issue, but the data is structured the same.
    - Tries to use char arrays instead of the String library to save RAM.
    - Does not fully support the python scripts for downloading and deleting files from the sd card
    - EEPROM can be programmed by the main program - type "Settings" (with the serial terminal set to \n as new line) before the detector finishes starting up.
    - The screen has a day counter as well as hours, minutes and seconds

   Libraries required:
   Builtin:
    Wire
    SD
    EEPROM
   Need to be installed:
    U8g2 - u8x8 text only part required
    This can be found by searching for it in the libraries manager (sketch > include library > manage libraries)

   This file contains the main code.
   The defines.h file contains parameters that can be adjusted to adapt this to your needs and preferences.
   The strings.h file contains most of the strings in this program, so change these if you want to change the wording.
    - You might have to fiddle around a bit with u8x8.draw1x2String(x coordinate (0 to 15),y coordinate (0 to 7),text); at various parts of this program if the spacing goes weird.
*/
//Include the defines.h file in the same directory
#include "defines.h"
#include "strings.h"

#ifdef useScreen
//LCD Libraries. This one is slightly smaller than the adafruit one, although with a few less features.
#include <Wire.h>
#include <U8x8lib.h>
#endif
//SD card
#ifdef useSdCard
#include <SD.h>
File myFile;
#endif
#ifdef useScreen
U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(U8X8_PIN_NONE);
#endif
#include <EEPROM.h>
bool isMaster = true;
unsigned long count = 0;
float sipmVoltage = 0;

//Timing
unsigned int startTime = 0;
unsigned long totalDeadtime = 0;
#ifdef useSdCard
//SD Card stuff
bool isSDCard = false;
char separatorChar = ' ';
#endif

void setup() {
  //Set up ADC
  analogReference (EXTERNAL); //DO NOT REMOVE THIS LINE!!!!!!!!!!!!!!
  //It will probably blow up the microcontroller if an analogRead() is called before the reference is set properly.
  ADCSRA &= ~(bit (ADPS0) | bit (ADPS1) | bit (ADPS2));  // clear prescaler bits
  ADCSRA |= bit (ADPS0) | bit (ADPS1);                   // Set prescaler to 8
  //Make the led pin an output
  //pinMode(ledPin, OUTPUT);
  //pinMode(coincidencePin, INPUT);
  DDRD = (DDRD | ledPin) & (~coincidencePin); //Set the led to be an output and the coincidence pin to be an input
  if ((PIND & coincidencePin) == coincidencePin) { //If the coincidence pin is high, then put into slave mode
    isMaster = false;
    PORTD = PORTD | ledPin; //Turn the led on
  } else {
    //pinMode(coincidencePin, OUTPUT);
    //digitalWrite(coincidencePin,HIGH);
    DDRD = DDRD | coincidencePin; //Make the coincidence pin an output
    PORTD = PORTD | coincidencePin; //Turn the coincidence pin high
  }
#ifdef useScreen
  //Start the display
  u8x8.begin();
  u8x8.setFlipMode(1);
  u8x8.setPowerSave(0);
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.setContrast(EEPROM.read(contrastAddress));
#endif
  //Put screen upside down
#ifdef useSerial
  //Set up serial
  Serial.begin(9600);
  Serial.setTimeout(3000);
#endif
  char charBuffer[67] = ""; //Define as an empty string to keep the compiler happy.
#if defined useScreen || defined useSerial
  //Strings stored in PROGMEM need to be copied to RAM before they can be used. charBuffer is a temporary spot for them.
  strcpy_P(charBuffer, cosmicString);
#ifdef useScreen
  u8x8.draw2x2String(0, 0, charBuffer);
#endif
#ifdef useSerial
  Serial.write(charBuffer);
  Serial.write(' ');
#endif
  strcpy_P(charBuffer, watchString);
#ifdef useScreen
  u8x8.draw2x2String(6, 2, charBuffer);
  getEepromString(deviceID1Address,16,charBuffer);
  u8x8.draw1x2String(0,4,charBuffer);
  byte secondRowLength = getEepromString(deviceID2Address,16,charBuffer);
  u8x8.draw1x2String(17 - secondRowLength,6,charBuffer);
#endif
#ifdef useSerial
  Serial.write(charBuffer);
  strcpy_P(charBuffer, muonString);
  Serial.println(charBuffer);
  strcpy_P(charBuffer, deviceIdentity);
  Serial.write(charBuffer);
  getEepromString(deviceID1Address,eepromStringLength,charBuffer);
  Serial.write(charBuffer);
  getEepromString(deviceID2Address,eepromStringLength,charBuffer);
  Serial.println(charBuffer);
  strcpy_P(charBuffer, firmwareString);
  Serial.println(charBuffer);
  strcpy_P(charBuffer, compiledString);
  Serial.println(charBuffer);
  strcpy_P(charBuffer, aboutString1);
  Serial.println(charBuffer);
  strcpy_P(charBuffer, aboutString2);
  Serial.write(charBuffer);
  strcpy_P(charBuffer, aboutString3);
  Serial.println(charBuffer);
  #ifdef useSerialSettings
  strcpy_P(charBuffer, settingsInstructions);
  Serial.write(charBuffer);
  strcpy_P(charBuffer, settingsAndNL);
  Serial.write(charBuffer);
  strcpy_P(charBuffer, settingsInstructions2);
  Serial.println(charBuffer);
  #endif
  Serial.println();
#endif
#endif
  delay(1000);
#ifdef useSdCard
  //Set up the sd card
  if (SD.begin(sdCardPin)) {
    char fileExtension[5];
    if(EEPROM.read(formatAddress) == 0) {
      strcpy_P(fileExtension,txtString);
      //Separator char is already set
    } else {
      strcpy_P(fileExtension,csvString);
      separatorChar = ',';
    }
#if defined useScreen || defined useSerial
    strcpy_P(charBuffer, sdPresent);
#ifdef useSerial
    Serial.println(charBuffer);
#endif
    //Put here as sd.begin has a time delay
#ifdef useScreen
    u8x8.clear();
    u8x8.draw1x2String(0, 0, charBuffer);
#endif
    strcpy_P(charBuffer, loggingTo);
#ifdef useScreen
    u8x8.draw1x2String(0, 2, charBuffer);
#endif
#ifdef useSerial
    Serial.print(charBuffer);
#endif
#endif
    isSDCard = true;
    //Now go through the process of trying to find an empty file name to save
    //We want i to be accessable outside the loop, so do not use a for loop.
    byte i = 0;
    while (true) {
      //Write the filename with the right number into charBuffer using a reference
      filenameOfI(i, charBuffer, fileExtension);
      if (!SD.exists(charBuffer)) {
        break;
      }
      i++;
      if (i > maxNumberOfFiles) {
        i = 0;
        //Write the filename with the right number into charBuffer using a reference
        filenameOfI(0, charBuffer, fileExtension);
        break;
      }
    }
    //Get rid of 5 files following so can tell where up to if looped around a few times.
    deleteFiles(i + 1, fileExtension);
    myFile = SD.open(charBuffer, FILE_WRITE);
    if (myFile != true) {
      // It failed too, so give up.
      isSDCard = false;
      strcpy_P(charBuffer, fileError);
    }
#ifdef useSerial
    Serial.println(charBuffer);
#endif
#ifdef useScreen
    u8x8.draw1x2String(0, 4, charBuffer);
#endif
  } else {
    strcpy_P(charBuffer, noSdCard);
#ifdef useSerial
    Serial.println(charBuffer);
#endif
    //Put here as sd.begin has a time delay
#ifdef useScreen
    u8x8.clear();
    u8x8.draw1x2String(0, 0, charBuffer);
#endif
  }
#ifdef useSdCard
  if (isSDCard) {
    strcpy_P(charBuffer, cosmicString);
    myFile.write(charBuffer);
    myFile.write(' ');
    strcpy_P(charBuffer, watchString);
    myFile.write(charBuffer);
    strcpy_P(charBuffer, muonString);
    myFile.println(charBuffer);
    strcpy_P(charBuffer, deviceIdentity);
    myFile.write(charBuffer);
    getEepromString(deviceID1Address,eepromStringLength,charBuffer);
    myFile.write(charBuffer);
    getEepromString(deviceID2Address,eepromStringLength,charBuffer);
    myFile.println(charBuffer);
    strcpy_P(charBuffer, firmwareString);
    myFile.println(charBuffer);
    strcpy_P(charBuffer, compiledString);
    myFile.println(charBuffer);
    strcpy_P(charBuffer, aboutString1);
    myFile.println(charBuffer);
    strcpy_P(charBuffer, aboutString2);
    myFile.write(charBuffer);
    strcpy_P(charBuffer, aboutString3);
    myFile.println(charBuffer);
    myFile.println();
    //Will flush the file in a little bit.
  }
#endif
#endif
#if defined useScreen || defined useSerial || defined useSdCard
  if (isMaster) {
    strcpy_P(charBuffer, masterString);
  } else {
    strcpy_P(charBuffer, slaveString);
  }
#endif
#ifdef useScreen
  u8x8.draw1x2String(0, 6, charBuffer);
#endif
#ifdef useSdCard
  if (isSDCard) {
    myFile.println(charBuffer);
    myFile.flush();
  }
#endif
#ifdef useSerial
  Serial.println(charBuffer);
#endif
  delay(1000);
  strcpy_P(charBuffer, txtHeader);
#ifdef useSerial
  Serial.println(charBuffer);
#endif
#ifdef useSdCard
  if (isSDCard) {
    //Get the header with commas instead of spaces if saving as a csv
    if(separatorChar == ',') {
      strcpy_P(charBuffer, csvHeader);
    }
    myFile.println(charBuffer);
    myFile.flush();
  }
#endif
  if (isMaster) {
    //digitalWrite(coincidencePin,LOW);
    PORTD = PORTD & (~coincidencePin); //Make the coincidence pin low
  }
  PORTD = PORTD & (~ledPin); //Make the led pin low
#ifdef useScreen
  //Set up the display for normal viewing
  const byte offsetColon1[] = {B00110000,0,0,0,0,0,0,0}; //Char that puts a colon on the left side to make it appear as a colon and a space
  const byte offsetColon2[] = {B00001100,0,0,0,0,0,0,0}; //Char that puts a colon on the left side to make it appear as a colon and a space
  u8x8.clear();
  strcpy_P(charBuffer, totalCount);
  u8x8.draw1x2String(0, 0, charBuffer);
  u8x8.drawTile(5,0,1,offsetColon1);
  u8x8.drawTile(5,1,1,offsetColon2);
  strcpy_P(charBuffer, upTime);
  u8x8.draw1x2String(0, 2, charBuffer);
  u8x8.drawTile(4,2,1,offsetColon1);
  u8x8.drawTile(4,3,1,offsetColon2);
  if (isMaster) {
    u8x8.draw1x2Glyph(0, 4, masterChar);
  } else {
    u8x8.draw1x2Glyph(0, 4, slaveChar);
  }
  strcpy_P(charBuffer, rateString);
  u8x8.draw1x2String(0, 6, charBuffer);
  u8x8.drawTile(4,6,1,offsetColon1);
  u8x8.drawTile(4,7,1,offsetColon2);
#endif
#if defined useSerial && defined useSerialSettings
  enterSettings();
#endif
  analogRead(detectorPin);
  startTime = millis();
}

void loop() {
  if (analogRead(detectorPin) > signalThreshold) {
    int detectionADC = analogRead(detectorPin);
    unsigned long timeStamp = micros();
    unsigned long timeDetecting = millis() - startTime;
    bool usePulse = false;
    //digitalWrite(ledPin,HIGH);
    //We detected something!
    if (isMaster) {
      PORTD = PORTD | coincidencePin; //Make the led pin high
      usePulse = true;
    }
#if defined useScreen || defined useSerial
    //Swap the adc to the temperature pin and measure a few times to stabilise
    //Also delay a while to allow the coincidence pin to stay high and allow master detector to trigger if slave.
    /*Serial.println("About to read temp");*/
    analogRead(temperaturePin); //Swap the adc to the temperature pin and throw away the first measurement as it might be off by a bit
    float temperatureC = (((analogRead(temperaturePin) + analogRead(temperaturePin)) * (1650. / 1024)) - tempSenseOffset) / 10. ;
    //adc * 3300 - 1024 * tempSenseOffset / 10
    /*Serial.print("Temp is: ");
      Serial.println(temperatureC);*/
    //Make the text for the file and serial without using the String library to save RAM
#endif
    if (!isMaster) {
      if ((PIND & coincidencePin) == coincidencePin) {
        usePulse = true;
      }
    }
    if (usePulse) {
      /*Serial.println("Using Pulse");*/
      count++;
      //digitalWrite(ledPin,HIGH);
      PORTD = PORTD | ledPin; //Make the led pin high
#if defined useScreen || defined useSerial
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
      ltoa(count, countString, 10);
      ltoa(timeDetecting, timeString, 10);
      itoa(detectionADC, adcString, 10);
      byte decimalPlaces = 1;
      if (sipmVoltage >= 100 || sipmVoltage < 0) {
        decimalPlaces = 0;
      }
      dtostrf(sipmVoltage, 1, decimalPlaces, sipmVString);
      ltoa(totalDeadtime, deadtimeString, 10);
      dtostrf(temperatureC, 1, 1, tempCString);
#ifdef useSerial
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
      Serial.write("\r\n");
#endif
#ifdef useSdCard
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
        myFile.write("\r\n");
        myFile.flush();
      }
#endif
#endif
    }
    //digitalWrite(ledPin,LOW);
    PORTD = PORTD & (~ledPin); //Make the led pin low
    //Wait until the pulse falls back so does not trigger multiple times for the one muon.
    while (analogRead(detectorPin) > resetThreshold) {
    }

    if (isMaster) {
      //digitalWrite(coincidencePin,LOW);
      PORTD = PORTD & (~coincidencePin); //Make the coincidence pin low
    }
    totalDeadtime += (micros() - timeStamp) / 1000;
  }
#ifdef useScreen
  updateTime();
#endif
}
#ifdef useScreen
void updateTime() {
  //Update the clock and rate on the display every second.
  static unsigned long nextTime = 0;
  if (millis() >= nextTime) {
    nextTime = millis();
    unsigned long timeOfUpdate = nextTime;
    //u8x8.setCursor(0,2);
    char charBuffer[12]; //10 bytes should be enough, but add a bit more 999:00:00 is about a month and a bit, 9999:00:00 is over a year - millis() rolls over after 50 days - possibly add stuff in to handle this?
    //Time:dd:hh:mm:ss change to?
    runningTime(charBuffer);
    u8x8.draw1x2String(5, 2, charBuffer);
    //Calculate the count rate and standared deviation
    float countRate, countStd;
    if (count > 0) {
      countRate = float(count) / (float(nextTime - startTime - totalDeadtime) / 1000.0);
      countStd = sqrt(count) / ((nextTime - startTime - totalDeadtime) / 1000.0);
    } else {
      countRate = 0;
      countStd = 0;
    }
    //Do stuff with these numbers
    byte decimalPlaces = 2;
    if (countRate < 10) {
      decimalPlaces = 3;
    }
    dtostrf(countRate, 5, decimalPlaces, charBuffer);
    u8x8.draw1x2String(5, 6, charBuffer);
    //strcpy_P(charBuffer,plusOrMinus);
    //Draw a small + symbol above a small - symbol to make a +/- sign
    u8x8.drawGlyph(10, 6, '+');
    u8x8.drawGlyph(10, 7, '-');
    //u8x8.draw1x2String(11,6,charBuffer);
    dtostrf(countStd, 5, decimalPlaces, charBuffer);
    u8x8.draw1x2String(11, 6, charBuffer);
    updateScreen();
    nextTime = millis();
    totalDeadtime += nextTime - timeOfUpdate;
    nextTime += screenUpdateTime;
  }
}
#endif
#ifdef useScreen
void updateScreen() {
  //Only update the count and pulse size part of the display when required
  static unsigned long lastCount = 90; //Some value other than 0
  if (count != lastCount) {
    lastCount = count;
    char charBuffer[16];
    ltoa(count, charBuffer, 10);
    u8x8.draw1x2String(6, 0, charBuffer);
    //Draw a line indicating the size of the pulse on the u8x8 display
    if (count == 0) {
      strcpy_P(charBuffer, detectorReady);
    } else if (sipmVoltage > largePulseThreshold) {
      strcpy_P(charBuffer, sipmWow);
    } else {
      byte i = 0;
      for (; (i < (sipmVoltage + 10) / 10) && (i < 15); i++) {
        charBuffer[i] = barChartChar;
      }
      //Fill the rest up with spaces to cover the previous graph
      for (; i < 15; i++) {
        charBuffer[i] = ' ';
      }
      //Null terminate the array
      charBuffer[i] = 0;
    }
    u8x8.draw1x2String(1, 4, charBuffer);
  }
}
#endif
// This function converts the measured ADC value to a SiPM voltage via the calibration array
float get_sipm_voltage(float adc_value) {
  float voltage = 0;
  for (int i = 0; i < (sizeof(cal) / sizeof(float)); i++) {
    voltage += cal[i] * pow(adc_value, (sizeof(cal) / sizeof(float) - i - 1));
  }
  return voltage;
}
#ifdef useScreen
void runningTime(char * timeSinceStart) {
  unsigned long milliSeconds = millis() - startTime;
  byte seconds = milliSeconds / 1000 % 60;
  byte minutes = milliSeconds / 60000 % 60;
  byte hours = milliSeconds / 3600000 % 24;
  byte days = milliSeconds / 86400000; //Days
  
  char number[7];
  itoa(days, number, 10);
  strcpy(timeSinceStart, number);
  strcat_P(timeSinceStart, colon);
  //Add a 0 to keep places
  if (hours < 10) {
    strcat_P(timeSinceStart, zero);
  }
  itoa(hours, number, 10);
  strcat(timeSinceStart, number);
  strcat_P(timeSinceStart, colon);
  //Add a 0 to keep places
  if (minutes < 10) {
    strcat_P(timeSinceStart, zero);
  }
  itoa(minutes, number, 10);
  strcat(timeSinceStart, number);
  strcat_P(timeSinceStart, colon);
  if (seconds < 10) {
    strcat_P(timeSinceStart, zero);
  }
  itoa(seconds, number, 10);
  strcat(timeSinceStart, number);
}
#endif
#ifdef useSdCard
void deleteFiles(byte index, char* extension) {
  char charBuffer[12];
  for (byte i = index; i < index + numberOfFilesToDelete; i++) {
    filenameOfI(i, charBuffer, extension);
    if (SD.exists(charBuffer)) {
      Serial.print(F("Removing "));
      Serial.println(charBuffer);
      SD.remove(charBuffer);
    }
  }
}
#endif
#ifdef useSdCard
void filenameOfI(byte i, char * returnBuffer, char * extension) {
  //Copy file name template from flash into ram
  strcpy_P(returnBuffer, filename);
  //Split i into a three digit number and then convert to ASCII by adding 48
  returnBuffer[fileNumberStartPoint] = (i / 100) + 48;
  returnBuffer[fileNumberStartPoint + 1] = ((i / 10) % 10) + 48;
  returnBuffer[fileNumberStartPoint + 2] = (i % 10) + 48;
  returnBuffer[fileNumberStartPoint + 3] = 0; //Null terminate it
  strcat(returnBuffer,extension);
}
#endif
#if defined useScreen || defined useSerial || defined useSdCard
int getEepromString(int startAddress, int maxChars, char * returnBuffer) {
  int endAddress = maxChars + startAddress;
  int returnBufferCount = 0;
  //Just to make sure that someone does not try to access eeprom that does not exist
  if(endAddress >= EEPROM.length()) {
    endAddress = EEPROM.length() - 1;
  }
  for(;startAddress < endAddress;startAddress++) {
    returnBuffer[returnBufferCount] = EEPROM.read(startAddress);
    if(returnBuffer[returnBufferCount] == 0) {
      return returnBufferCount + 1;
    }
    returnBufferCount++;
  }
  //Null terminate what we have
  returnBuffer[returnBufferCount] = 0;
  return returnBufferCount;
}
#endif
#if defined useSerialSettings && defined useSerial
void enterSettings() {
  //Only bother checking if something was sent to the arduino
  if(Serial.available()) {
    PORTD = PORTD | ledPin; //Turn the led on
    Serial.setTimeout(serialTimeout);
    char charBuffer[80];
    //if "Settings" was sent
    byte charsRead = Serial.readBytesUntil('\n',charBuffer,40);
    charBuffer[charsRead] = 0; //Null terminate the array
    if(charsRead != 0) {
      char compareBuffer[9];
      strcpy_P(compareBuffer, settingsString);
      if(strcmp(charBuffer,compareBuffer) == 0) {
        //Print out the prompts for what to do next
        strcpy_P(charBuffer, settingsHeading);
        Serial.write(charBuffer);
        strcpy_P(charBuffer, resetToExit);
        Serial.println(charBuffer);
  #ifdef useScreen
        u8x8.clear();
        u8x8.setInverseFont(1);
        //Fille the screen with white to make look different - might be useful to check all of the display is working properly
        for(byte y = 2; y < 8; y++) {
          for(byte x = 0; x < 16; x++) {
            u8x8.drawGlyph(x,y,' ');
          }
        }
        u8x8.draw2x2String(0, 0, compareBuffer);
        u8x8.draw1x2String(0,3,charBuffer);
  #endif
        strcpy_P(charBuffer, settingsHeading1);
        Serial.write(charBuffer);
        strcpy_P(charBuffer, settingsAndNL);
        Serial.println(charBuffer);
        strcpy_P(charBuffer, settingsHeading2);
        Serial.write(charBuffer);
        strcpy_P(charBuffer, settingsAndNL);
        Serial.println(charBuffer);
        strcpy_P(charBuffer, settingsHeading3);
        Serial.write(charBuffer);
        strcpy_P(charBuffer, settingsHeading4);
        Serial.write(charBuffer);
        strcpy_P(charBuffer, settingsAndNL);
        Serial.println(charBuffer);
        strcpy_P(charBuffer, settingsHeading5);
        Serial.write(charBuffer);
        strcpy_P(charBuffer, settingsAndNL);
        Serial.println(charBuffer);
        strcpy_P(charBuffer, mistakeText);
        Serial.println(charBuffer);
        while(true) {
          //Wait until something else is sent
          clearSerialBuffer();
          PORTD = PORTD | ledPin; //Turn the led on
          while(!Serial.available());
          PORTD = PORTD & (~ledPin); //Make the led pin low
          strcpy_P(charBuffer, settingsString);
          //if Something was sent
          charsRead = Serial.readBytesUntil(' ', charBuffer, 40);
          charBuffer[charsRead] = 0;
          if(charsRead != 0) {
            bool isSuccess = false;
            strcpy_P(compareBuffer, id1String);
            //If it matches
            if(strcmp(charBuffer,compareBuffer) == 0) {
              charsRead = Serial.readBytesUntil('\n', charBuffer, 40);
              charBuffer[charsRead] = 0;
              writeToEeprom(deviceID1Address,charBuffer);
              isSuccess = true;
            }
            strcpy_P(compareBuffer, id2String);
            //If it matches
            if(strcmp(charBuffer,compareBuffer) == 0) {
              charsRead = Serial.readBytesUntil('\n', charBuffer, 40);
              charBuffer[charsRead] = 0;
              writeToEeprom(deviceID2Address,charBuffer);
              isSuccess = true;
            }
            strcpy_P(compareBuffer, formatString);
            //If it matches
            if(strcmp(charBuffer,compareBuffer) == 0) {
              charsRead = Serial.readBytesUntil('\n', charBuffer, 40);
              charBuffer[charsRead] = 0;
              strcpy_P(compareBuffer, txtString);
              if(strcmp(charBuffer,compareBuffer) == 0) {
                EEPROM.update(formatAddress,0);
              } else {
                EEPROM.update(formatAddress,1);
              }
              isSuccess = true;
            }
            strcpy_P(compareBuffer, contrastString);
            //If it matches
            if(strcmp(charBuffer,compareBuffer) == 0) {
              charsRead = Serial.parseInt();
              EEPROM.update(contrastAddress,charsRead);
  #ifdef useScreen
              u8x8.setContrast(charsRead);
  #endif
              isSuccess = true;
            }
            if(isSuccess) {
              strcpy_P(charBuffer,experiencedSuccess);
            } else {
              strcpy_P(charBuffer,noSuccess);
            }
            Serial.println(charBuffer);
          }
        }
      }
    }
    delay(10000);
    PORTD = PORTD & (~ledPin); //Make the led pin low
  }
}
//Write a string to eeprom starting from an address and stopping when running out of eeprom to write or the whole string is written
void writeToEeprom(int startAddress, char* charArray) {
  int count = 0;
  for(int i = startAddress; i < EEPROM.length(); i++) {
    EEPROM.update(i,charArray[count]);
    if(charArray[count] == 0) {
      break;
    }
    count++;
  }
}
void clearSerialBuffer() {
  while(Serial.available()) {
   Serial.read();
  }
}
#endif
