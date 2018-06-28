/* This is still a Work In Progress! There may be a few bugs.
 *  
 * Non Official Version of the CosmicWatch Desktop Muon Detector Arduino Code
 * 
 * The original code and documentation / designs can be found here:
 * https://github.com/spenceraxani/CosmicWatch-Desktop-Muon-Detector-v2
 * 
 * This software uses both the oled display and the sd card if they are fitted.
 * Differences to the original version are mainly:
 *  - Using a smaller library for the driving the display to save RAM.
 *  - By default saves files in CSV format so can be opened in excel without having to be imported
 *  - Tries to use char arrays instead of the String library to save RAM.
 *  - Does not fully support the python scripts for connecting these devices to a computer.
 * 
 * Uses a different library by Bill Grieman for the driving the display than the official version that is much smaller.
 * It can be downloaded here: https://github.com/greiman/SSD1306Ascii
 * 
 * This file contains the main code.
 * The defines.h file contains parameters that can be adjusted to adapt this to your needs and preferences.
 * The strings.h file contains most of the strings in this program, so change these if you want to change the wording.
 *  - You might have to fiddle around a bit with oled.setCursor(x,y); at various parts of this program if the spacing goes weird.
 */
//Include the defines.h file in the same directory
#include "defines.h"
#include "strings.h" 

//LCD Libraries. This one is slightly smaller than the adafruit one, although with a few less features.
#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
//SD card
#include <SD.h>

File myFile;
SSD1306AsciiWire oled;

bool isMaster = true;
unsigned long count = 0;
//Timing
unsigned int startTime = 0;
unsigned long totalDeadtime = 0; 
//SD Card stuff
bool isSDCard = false;

void setup() {
  //Set up ADC
  analogReference (EXTERNAL);
  ADCSRA &= ~(bit (ADPS0) | bit (ADPS1) | bit (ADPS2));    // clear prescaler bits
  //ADCSRA |= bit (ADPS1);                                   // Set prescaler to 4  
  ADCSRA |= bit (ADPS0) | bit (ADPS1); // Set prescaler to 8
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

  //Start the display
  Wire.begin();
  Wire.setClock(400000L);
  oled.begin(&Adafruit128x64, 0x3C);
  //Put screen upside down
  oled.displayRemap(true);
  //Set up serial
  Serial.begin(9600);
  Serial.setTimeout(3000);
  char charBuffer[67]; //Originally 67
  //Set up the display
  //oled.setFont(fixed_bold10x15);
  oled.setFont(Cooper26);
  //oled.set2X();
  oled.clear();
  //Strings stored in PROGMEM need to becopiedto RAM before they can be used. charBuffer is a temporary spot for them.
  strcpy_P(charBuffer, cosmicString);
  oled.println(charBuffer);
  Serial.print(charBuffer);
  Serial.write(' ');
  oled.setCursor(40,4);
  strcpy_P(charBuffer, watchString);
  oled.print(charBuffer);
  Serial.println(charBuffer);
  delay(1000);
  //oled.setFont(X11fixed7x14B);
  oled.setFont(Callibri15);
  oled.set1X();
  //Set up the sd card
  if (SD.begin(sdCardPin)) {
    strcpy_P(charBuffer,sdPresent);
    Serial.println(charBuffer);
    //Put here as sd.begin has a time delay
    oled.clear();
    oled.println(charBuffer);
    strcpy_P(charBuffer,loggingTo);
    oled.println(charBuffer);
    Serial.print(charBuffer);
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
      Serial.println(F("File error. Giving up."));
      isSDCard = false;
    }
    Serial.println(charBuffer);
    oled.println(charBuffer);
  } else {
    strcpy_P(charBuffer,noSdCard);
    Serial.println(charBuffer);
    //Put here as sd.begin has a time delay
    oled.clear();
    oled.println(charBuffer);
  }
  if(isMaster) {
    strcpy_P(charBuffer, masterString);
  } else {
    strcpy_P(charBuffer, slaveString);
  }
  oled.setCursor(0,12);
  oled.print(charBuffer);
  if(isSDCard) {
    myFile.println(charBuffer);
    myFile.flush();
  }
  delay(1000);
  strcpy_P(charBuffer,csvHeader);
  Serial.println(charBuffer);
  if(isSDCard) {
    myFile.println(charBuffer);
    myFile.flush();
  }
  if(isMaster) {
    digitalWrite(coincidencePin,LOW);
  }
  digitalWrite(ledPin,LOW);
  //Set up the display for normal viewing
  oled.clear();
  strcpy_P(charBuffer,totalCount);
  oled.print(charBuffer);
  oled.println('0');
  oled.setCursor(0,2);
  strcpy_P(charBuffer,upTime);
  oled.println(charBuffer);
  if(isMaster) {
    oled.println(masterChar);
  } else {
    oled.println(slaveChar);
  }
  oled.setCursor(0,6);
  strcpy_P(charBuffer,rateString);
  oled.print(charBuffer);
  analogRead(A0);
  startTime = millis();
}

void loop() {
  if(analogRead(detectorPin) > signalThreshold) {
  
    int detectionADC = analogRead(detectorPin);
    unsigned long timeStamp = millis();
    bool usePulse = false;
    //We detected something!
    /*if(isMaster) {
      digitalWrite(coincidencePin, HIGH);
      count++;
      usePulse = true;
    } else if(digitalRead(coincidencePin) == HIGH) {
      usePulse = true;
      count++;
    }*/
    if(isMaster) {
      digitalWrite(coincidencePin, HIGH);
      count++;
      usePulse = true;
    }
    //Swap the adc to the temperature pin and measure a few times to stabilise
    //Also delay a while to allow the coincidence pin to stay high and allow master detector to trigger if slave.
    analogRead(temperaturePin);
    analogRead(temperaturePin);
    float temperatureC = (((analogRead(temperaturePin)+analogRead(temperaturePin)+analogRead(temperaturePin))/3. * (3300./1024)) - 500)/10. ;
    //Make the text for the file and serial without using the String library to save RAM

    if(!isMaster) {
     if(digitalRead(coincidencePin) == HIGH) {
      usePulse = true;
      count++;
      } 
    }
    if(usePulse) {
      digitalWrite(ledPin,HIGH);
      //In the form of:
      //count,timeStamp,detectionADC,SIPM Voltage,deadTime,temperature
      //dead time is not yet implimented, so is zero for now.
      
      //When printing to the display, we only want to update the areas that need to change.
      //Uses set cursor a lot to set position of start of number.
      //May need to fiddle with to get spacing right.
      char charBuffer[50];
      //strcpy_P(charBuffer,totalCount);
      //oled.home();
      //oled.print(charBuffer);
      oled.setCursor(72,0);
      ltoa(count,charBuffer,10);
      oled.println(charBuffer);
      //Time row left blank here and updated later
      //oled.println();
      strcat_P(charBuffer,comma);
      char numberString[13];
      ltoa(timeStamp - startTime,numberString,10);
      strcat(charBuffer,numberString);
      strcat_P(charBuffer,comma);
      itoa(detectionADC,numberString,10);
      strcat(charBuffer,numberString);
      strcat_P(charBuffer,comma);
      //strcpy_P(numberString,sipmVolts);
      //oled.print(numberString);
      float sipmVoltage = get_sipm_voltage(detectionADC);
      //Limit to max values so can be handled by dtostrf
      byte decimalPlaces = 1;
      if(sipmVoltage >= 100 || sipmVoltage < 0.0) {
        decimalPlaces = 0;
      }
      dtostrf(sipmVoltage, 4, decimalPlaces, numberString);
      strcat(charBuffer,numberString);
        //Serial.println("NotBig");
      strcat_P(charBuffer,comma);
      ltoa(totalDeadtime,numberString,10);
      strcat(charBuffer,numberString);
      strcat_P(charBuffer,comma);
      dtostrf(temperatureC, 5, 1, numberString);
      strcat(charBuffer,numberString);
      //Export the string to somewhere - serial and sd card if connected
      Serial.println(charBuffer);
      if(isSDCard) {
        myFile.println(charBuffer);
        myFile.flush();
      }
      //Draw a line indicating the size of the pulse on the oled display
      oled.setCursor(14,4);
      oled.clearToEOL();
      if(sipmVoltage > largePulseThreshold) {
        strcpy_P(charBuffer,sipmWow);
        oled.print(charBuffer);
      } else {
          //Get rid of spaces between chars 
          oled.setLetterSpacing(0);
          for (int i = 1; i <=  (sipmVoltage + 10) / 5; i++) {
            oled.print(barChartChar);
          }
          oled.setLetterSpacing(1);
      }
      digitalWrite(ledPin,LOW);
    }
    //Wait until the pulse falls back so does not trigger multiple times for the one muon.
    while(analogRead(detectorPin > resetThreshold)) {
      updateTime();
    }
    if(isMaster) {
      digitalWrite(coincidencePin, LOW);
    }
    totalDeadtime += millis() - timeStamp;
  }
  updateTime();
}

void updateTime() {
  //Update the clock and rate on the display every second.
  static unsigned long nextTime = 0;
  if(millis() >= nextTime) {
    nextTime = millis();
    unsigned long timeOfUpdate = nextTime;
    //oled.setCursor(0,2);
    char charBuffer[10];
    //strcpy_P(charBuffer,upTime);
    //oled.print(charBuffer);
    oled.setCursor(51,2);
    oled.print(runningTime());
    //Calculate the count rate and standared deviation
    float countRate,countStd;
    if(count > 0) {
      countRate = float(count) / (float(nextTime - startTime - totalDeadtime) / 1000.0);
      countStd = sqrt(count) / ((nextTime - startTime - totalDeadtime) / 1000);
    } else {
      countRate = 0;
      countStd = 0;
    }
    //Do stuff with these numbers
    /*oled.setCursor(0,6);
    strcpy_P(charBuffer,rateString);
    oled.print(charBuffer);*/
    oled.setCursor(33,6);
    byte decimalPlaces = 2;
    if (countRate < 10) {
      decimalPlaces = 3;
    }
    dtostrf(countRate, 5, decimalPlaces, charBuffer);
    oled.print(charBuffer);
    strcpy_P(charBuffer,plusOrMinus);
    oled.print(charBuffer);
    dtostrf(countStd, 5, decimalPlaces, charBuffer);
    oled.print(charBuffer);
    nextTime = millis();
    totalDeadtime += nextTime - timeOfUpdate;
    nextTime += screenUpdateTime;
  }
}
// This function converts the measured ADC value to a SiPM voltage via the calibration array
float get_sipm_voltage(float adc_value)
{
  float voltage = 0;
  for (int i = 0; i < (sizeof(cal)/sizeof(float)); i++) {
    voltage += cal[i] * pow(adc_value,(sizeof(cal)/sizeof(float)-i-1));
  }
  /*//Limit to max values so can be handled by dtostrf
  if(voltage >= 100) {
    voltage = 99.9;
  } else if(voltage < 0.0) {
    voltage = 0;
  }*/
  return voltage;
}
char * runningTime() {
  unsigned long milliSeconds = millis() - startTime;
  byte seconds = milliSeconds / 1000 % 60;
  byte minutes = milliSeconds / 60000 % 60;
  unsigned int hours = milliSeconds / 3600000;
  static char timeSinceStart[7];
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
  return timeSinceStart;
}
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
void filenameOfI(byte i,char * returnBuffer) {
  //Copy file name template from flash into ram
  strcpy_P(returnBuffer, filename);
  //Split i into a three digit number and then convert to ASCII by adding 48
  returnBuffer[fileNumberStartPoint] = (i / 100) + 48;
  returnBuffer[fileNumberStartPoint + 1] = ((i / 10) % 10) + 48;
  returnBuffer[fileNumberStartPoint + 2] = (i % 10) + 48;
}
