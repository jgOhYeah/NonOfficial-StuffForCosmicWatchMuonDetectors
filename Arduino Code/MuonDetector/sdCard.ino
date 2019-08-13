/*oled.h
 * Code adapted from Spencer Axiani's original software and designs by Jotham Gates
 * This file contains functions relating to the display
 * See MuonDetector.ino for more information
 */
#ifdef USE_SD_CARD
void filenameOfI(byte i, char * returnBuffer, char * extension) {
  //Copy file name template from flash into ram
  strcpy_P(returnBuffer, filename);
  //Split i into a three digit number and then convert to ASCII by adding 48
  returnBuffer[FILENAME_NUMBER_START_POS] = (i / 100) + 48;
  returnBuffer[FILENAME_NUMBER_START_POS + 1] = ((i / 10) % 10) + 48;
  returnBuffer[FILENAME_NUMBER_START_POS + 2] = (i % 10) + 48;
  returnBuffer[FILENAME_NUMBER_START_POS + 3] = 0; //Null terminate it
  strcat(returnBuffer,extension);
}
#endif
#ifdef USE_SD_CARD
void deleteFiles(byte index, char* extension) {
  char charBuffer[12];
  for (byte i = index; i < index + NUMBER_FILES_TO_DELETE; i++) {
    filenameOfI(i, charBuffer, extension);
    if (SD.exists(charBuffer)) {
      Serial.print(F("Removing "));
      Serial.println(charBuffer);
      SD.remove(charBuffer);
    }
  }
}
void openLogFile(char* charBuffer) {
  char fileExtension[5];
  if(EEPROM.read(EEPROM_FILE_FORMAT) == 0) {
    strcpy_P(fileExtension,txtString);
    //Separator char is already set
  } else {
    strcpy_P(fileExtension,csvString);
    separatorChar = ',';
  }
  //We want i to be accessable outside the loop, so do not use a for loop.
  byte i = 0;
  while (true) { //Crashes in here:
    DEBUG_RAM();
    //Write the filename with the right number into charBuffer using a reference
    filenameOfI(i, charBuffer, fileExtension);
    //DEBUG_RAM();
    if (!SD.exists(charBuffer)) {
      break;
    }
    i++;
    if (i > MAX_FILES) {
      i = 0;
      //Write the filename with the right number into charBuffer using a reference
      filenameOfI(0, charBuffer, fileExtension);
      break;
    }
  }
  //Get rid of 5 files following so can tell where up to if looped around a few times.
  deleteFiles(i + 1, fileExtension);
  myFile = SD.open(charBuffer, FILE_WRITE);
  //Serial.println(charBuffer);
  if (myFile != true) {
    // It failed too, so give up.
    isSDCard = false;
    strcpy_P(charBuffer, fileError);
  }
}
#endif
