/* This is still a Work In Progress! There may be a few bugs.
 *  
 * Non Official Version of the CosmicWatch Desktop Muon Detector Arduino Code
 * 
 * The original code and documentation / designs can be found here:
 * https://github.com/spenceraxani/CosmicWatch-Desktop-Muon-Detector-v2
 * 
 * This software uses both the u8x8 display and the sd card if they are fitted.
 * Differences to the original version are mainly:
 *  - Using a smaller library for the driving the display to save RAM it is also fairly fast
 *  - By default saves files in CSV format so can be opened in excel without having to be imported
 *  - Tries to use char arrays instead of the String library to save RAM.
 *  - Does not fully support the python scripts for connecting these devices to a computer.
 * 
 * Libraries required:
 * Builtin:
 *  Wire
 *  SD
 * Need to be installed:
 *  U8g2 - u8x8 text only part required
 *  This can be found by searching for it in the libraries manager (sketch > include library > manage libraries)
 * 
 * This file contains the main code.
 * The defines.h file contains parameters that can be adjusted to adapt this to your needs and preferences.
 * The strings.h file contains most of the strings in this program, so change these if you want to change the wording.
 *  - You might have to fiddle around a bit with u8x8.draw1x2String(x coordinate (0 to 15),y coordinate (0 to 7),text); at various parts of this program if the spacing goes weird.
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

bool isMaster = true;
unsigned long count = 0;
float sipmVoltage = 0;

//Timing
unsigned int startTime = 0;
unsigned long totalDeadtime = 0; 
#ifdef useSdCard
//SD Card stuff
bool isSDCard = false;
#endif

void setup() {
  //Set up ADC
  analogReference (EXTERNAL); //DO NOT REMOVE THIS LINE!!!!!!!!!!!!!!
  //It will probably blow up the microcontroller if an analogRead() is called before the reference is set properly.
  ADCSRA &= ~(bit (ADPS0) | bit (ADPS1) | bit (ADPS2));  // clear prescaler bits
  ADCSRA |= bit (ADPS0) | bit (ADPS1);                   // Set prescaler to 8
  //Make the led pin an output
  pinMode(ledPin, OUTPUT); 
  pinMode(coincidencePin, INPUT);
  if(digitalRead(coincidencePin) == HIGH) {
    isMaster = false;
    digitalWrite(ledPin,HIGH);
  } else {
    pinMode(coincidencePin, OUTPUT);
    digitalWrite(coincidencePin,HIGH);
  }
#ifdef useScreen
  //Start the display
  u8x8.begin();
  u8x8.setFlipMode(1);
  u8x8.setPowerSave(0);
  u8x8.setFont(u8x8_font_chroma48medium8_r);
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
  u8x8.draw2x2String(0,1,charBuffer);
  #endif
  #ifdef useSerial
  Serial.write(charBuffer);
  Serial.write(' ');
  #endif
  strcpy_P(charBuffer, watchString);
  #ifdef useScreen
  u8x8.draw2x2String(6,5,charBuffer);
  #endif
  #ifdef useSerial
  Serial.write(charBuffer);
  strcpy_P(charBuffer, muonString);
  Serial.println(charBuffer);
  strcpy_P(charBuffer, deviceIdentity);
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
  Serial.println();
  #endif
#endif
  delay(1000);
#ifdef useSdCard
  //Set up the sd card
  if (SD.begin(sdCardPin)) {
  #if defined useScreen || defined useSerial
    strcpy_P(charBuffer,sdPresent);
    #ifdef useSerial
    Serial.println(charBuffer);
    #endif
    //Put here as sd.begin has a time delay
    #ifdef useScreen
    u8x8.clear();
    u8x8.draw1x2String(0,0,charBuffer);
    #endif
    strcpy_P(charBuffer,loggingTo);
    #ifdef useScreen
    u8x8.draw1x2String(0,2,charBuffer);
    #endif
    #ifdef useSerial
    Serial.print(charBuffer);
    #endif
  #endif
    isSDCard = true;
    //Now go through the process of trying to find an empty file name to save
    //We want i to be accessable outside the loop, so do not use a for loop.
    byte i = 0;
    while(true) {
      //Write the filename with the right number into charBuffer using a reference
      filenameOfI(i,charBuffer);
      if(!SD.exists(charBuffer)) {
        break;
      }
      i++;
      if(i > maxNumberOfFiles) {
        i = 0;
        //Write the filename with the right number into charBuffer using a reference
        filenameOfI(0,charBuffer);
        break;
      }
    }
    //Get rid of 5 files following so can tell where up to if looped around a few times.
    deleteFiles(i + 1);
    myFile = SD.open(charBuffer, FILE_WRITE);
    if(myFile != true) {
      // It failed too, so give up.
      isSDCard = false;
      strcpy_P(charBuffer,fileError);
    }
  #ifdef useSerial
    Serial.println(charBuffer);
  #endif
  #ifdef useScreen
    u8x8.draw1x2String(0,4,charBuffer);
  #endif
  } else {
    strcpy_P(charBuffer,noSdCard);
  #ifdef useSerial
    Serial.println(charBuffer);
  #endif
    //Put here as sd.begin has a time delay
  #ifdef useScreen
    u8x8.clear();
    u8x8.draw1x2String(0,0,charBuffer);
  #endif
  }
#ifdef useSdCard
  if(isSDCard) {
    myFile.write(charBuffer);
    strcpy_P(charBuffer, muonString);
    myFile.println(charBuffer);
    strcpy_P(charBuffer, deviceIdentity);
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
  if(isMaster) {
    strcpy_P(charBuffer, masterString);
  } else {
    strcpy_P(charBuffer, slaveString);
  }
#endif
#ifdef useScreen
  u8x8.draw1x2String(0,6,charBuffer);
#endif
#ifdef useSdCard
  if(isSDCard) {
    myFile.println(charBuffer);
    myFile.flush();
  }
#endif
  delay(1000);
  strcpy_P(charBuffer,csvHeader);
#ifdef useSerial
  Serial.println(charBuffer);
#endif
#ifdef useSdCard
  if(isSDCard) {
    myFile.println(charBuffer);
    myFile.flush();
  }
#endif
  if(isMaster) {
    digitalWrite(coincidencePin,LOW);
  }
  digitalWrite(ledPin,LOW);
#ifdef useScreen
  //Set up the display for normal viewing
  u8x8.clear();
  strcpy_P(charBuffer,totalCount);
  u8x8.draw1x2String(0,0,charBuffer);
  strcpy_P(charBuffer,upTime);
  u8x8.draw1x2String(0,2,charBuffer);
  if(isMaster) {
    u8x8.draw1x2Glyph(0,4,masterChar);
  } else {
    u8x8.draw1x2Glyph(0,4,slaveChar);
  }
  strcpy_P(charBuffer,rateString);
  u8x8.draw1x2String(0,6,charBuffer);
#endif
  analogRead(detectorPin);
  startTime = millis();
}

void loop() {
  if(analogRead(detectorPin) > signalThreshold) {
    int detectionADC = analogRead(detectorPin);
    unsigned long timeStamp = micros();
    unsigned long timeDetecting = millis() - startTime;
    bool usePulse = false;
    //digitalWrite(ledPin,HIGH);
    //We detected something!
    if(isMaster) {
      digitalWrite(coincidencePin, HIGH);
      usePulse = true;
    }
#if defined useScreen || defined useSerial
    //Swap the adc to the temperature pin and measure a few times to stabilise
    //Also delay a while to allow the coincidence pin to stay high and allow master detector to trigger if slave.
    /*Serial.println("About to read temp");*/
    analogRead(temperaturePin);
    analogRead(temperaturePin);
    float temperatureC = (((analogRead(temperaturePin)+analogRead(temperaturePin)+analogRead(temperaturePin))/3. * (3300./1024)) - tempSenseOffset)/10. ;
    //adc * 3300 - 1024 * tempSenseOffset / 10
    /*Serial.print("Temp is: ");
    Serial.println(temperatureC);*/
    //Make the text for the file and serial without using the String library to save RAM
#endif
    if(!isMaster) {
      if(digitalRead(coincidencePin) == HIGH) {
        usePulse = true;
      }
    }
    if(usePulse) {
      /*Serial.println("Using Pulse");*/
      count++;
      digitalWrite(ledPin,HIGH);
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
      ltoa(count,countString,10);
      ltoa(timeDetecting,timeString,10);
      itoa(detectionADC,adcString,10);
      byte decimalPlaces = 1;
      if(sipmVoltage >= 100 || sipmVoltage < 0) {
        decimalPlaces = 0;
      }
      dtostrf(sipmVoltage, 1, decimalPlaces, sipmVString);
      ltoa(totalDeadtime,deadtimeString,10);
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
      if(isSDCard) {
        myFile.write(countString);
        myFile.write(seperatorChar);
        myFile.write(timeString);
        myFile.write(seperatorChar);
        myFile.write(adcString);
        myFile.write(seperatorChar);
        myFile.write(sipmVString);
        myFile.write(seperatorChar);
        myFile.write(deadtimeString);
        myFile.write(seperatorChar);
        myFile.write(tempCString);
        myFile.write("\r\n");
        myFile.flush();
      }
  #endif
#endif
    }
    digitalWrite(ledPin,LOW);
    //Wait until the pulse falls back so does not trigger multiple times for the one muon.
    while(analogRead(detectorPin) > resetThreshold) { 
    }
    
    if(isMaster) {
      digitalWrite(coincidencePin,LOW);
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
  if(millis() >= nextTime) {
    nextTime = millis();
    unsigned long timeOfUpdate = nextTime;
    //u8x8.setCursor(0,2);
    char charBuffer[10];
    runningTime(charBuffer);
    u8x8.draw1x2String(5,2,charBuffer);
    //Calculate the count rate and standared deviation
    float countRate,countStd;
    if(count > 0) {
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
    u8x8.draw1x2String(5,6,charBuffer);
    //strcpy_P(charBuffer,plusOrMinus);
    //Draw a small + symbol above a small - symbol to make a +/- sign
    u8x8.drawGlyph(10,6,'+');
    u8x8.drawGlyph(10,7,'-');
    //u8x8.draw1x2String(11,6,charBuffer);
    dtostrf(countStd, 5, decimalPlaces, charBuffer);
    u8x8.draw1x2String(11,6,charBuffer);
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
  if(count != lastCount) {
    lastCount = count;
    char charBuffer[16];
    ltoa(count,charBuffer,10);
    u8x8.draw1x2String(6,0,charBuffer);
    //Draw a line indicating the size of the pulse on the u8x8 display
    if(count == 0) {
      strcpy_P(charBuffer,detectorReady);
    } else if(sipmVoltage > largePulseThreshold) {
      strcpy_P(charBuffer,sipmWow);
    } else {
        byte i = 0;
        for (; (i < (sipmVoltage + 10) / 10) && (i < 15); i++) {
          charBuffer[i] = barChartChar;
        }
        //Fill the rest up with spaces to cover the previous graph
        for(; i < 15; i++) {
          charBuffer[i] = ' ';
        }
        //Null terminate the array
        charBuffer[i] = 0;
    }
    u8x8.draw1x2String(1,4,charBuffer);
  }
}
#endif
// This function converts the measured ADC value to a SiPM voltage via the calibration array
float get_sipm_voltage(float adc_value) {
  float voltage = 0;
  for (int i = 0; i < (sizeof(cal)/sizeof(float)); i++) {
    voltage += cal[i] * pow(adc_value,(sizeof(cal)/sizeof(float)-i-1));
  }
  return voltage;
}
#ifdef useScreen
void runningTime(char * timeSinceStart) {
  unsigned long milliSeconds = millis() - startTime;
  byte seconds = milliSeconds / 1000 % 60;
  byte minutes = milliSeconds / 60000 % 60;
  unsigned int hours = milliSeconds / 3600000;
  char number[7];
  itoa(hours,number,10);
  strcpy(timeSinceStart,number);
  strcat_P(timeSinceStart,colon);
  //Add a 0 to keep places
  if(minutes < 10) {
    strcat_P(timeSinceStart,zero);
  }
  itoa(minutes,number,10);
  strcat(timeSinceStart,number);
  strcat_P(timeSinceStart,colon);
  if(seconds < 10) {
    strcat_P(timeSinceStart,zero);
  }
  itoa(seconds,number,10);
  strcat(timeSinceStart,number);
}
#endif
#ifdef useSdCard
void deleteFiles(byte index) {
  char charBuffer[12];
  for(byte i = index; i < index + numberOfFilesToDelete; i++) {
    filenameOfI(i,charBuffer);
    if(SD.exists(charBuffer)) {
      Serial.print(F("Removing "));
      Serial.println(charBuffer);
      SD.remove(charBuffer);
    }
  }
}
#endif
#ifdef useSdCard
void filenameOfI(byte i,char * returnBuffer) {
  //Copy file name template from flash into ram
  strcpy_P(returnBuffer, filename);
  //Split i into a three digit number and then convert to ASCII by adding 48
  returnBuffer[fileNumberStartPoint] = (i / 100) + 48;
  returnBuffer[fileNumberStartPoint + 1] = ((i / 10) % 10) + 48;
  returnBuffer[fileNumberStartPoint + 2] = (i % 10) + 48;
}
#endif
/*
int freeRam () 
{
extern int __heap_start, *__brkval; 
int v; 
return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}*/



