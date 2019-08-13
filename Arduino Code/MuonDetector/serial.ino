/*serial.ino
 * Code adapted from Spencer Axiani's original software and designs by Jotham Gates
 * This file contains functions relating to the serial connection to the computer and the EEPROM setting utility.
 * See MuonDetector.ino for more information
 */
#if defined USE_SERIAL_SETTINGS && defined USE_SERIAL
void enterSettings() {
  //Only bother checking if something was sent to the arduino
  if(Serial.available()) {
    PORTD = PORTD | PIN_LED; //Turn the led on
    Serial.setTimeout(SERIAL_TIMEOUT);
    char charBuffer[81];
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
  #ifdef USE_SCREEN
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
          PORTD = PORTD | PIN_LED; //Turn the led on
          while(!Serial.available());
          PORTD = PORTD & (~PIN_LED); //Make the led pin low
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
              writeToEeprom(EEPROM_ID1,charBuffer);
              isSuccess = true;
            }
            strcpy_P(compareBuffer, id2String);
            //If it matches
            if(strcmp(charBuffer,compareBuffer) == 0) {
              charsRead = Serial.readBytesUntil('\n', charBuffer, 40);
              charBuffer[charsRead] = 0;
              writeToEeprom(EEPROM_ID2,charBuffer);
              isSuccess = true;
            }
            strcpy_P(compareBuffer, formatString);
            //If it matches
            if(strcmp(charBuffer,compareBuffer) == 0) {
              charsRead = Serial.readBytesUntil('\n', charBuffer, 40);
              charBuffer[charsRead] = 0;
              strcpy_P(compareBuffer, txtString);
              if(strcmp(charBuffer,compareBuffer) == 0) {
                EEPROM.update(EEPROM_FILE_FORMAT,0);
              } else {
                EEPROM.update(EEPROM_FILE_FORMAT,1);
              }
              isSuccess = true;
            }
            strcpy_P(compareBuffer, contrastString);
            //If it matches
            if(strcmp(charBuffer,compareBuffer) == 0) {
              charsRead = Serial.parseInt();
              EEPROM.update(EEPROM_CONSTRAST,charsRead);
  #ifdef USE_SCREEN
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
    PORTD = PORTD & (~PIN_LED); //Make the led pin low
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
#if defined USE_SCREEN || defined USE_SERIAL || defined USE_SD_CARD
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
