/*oled.ino
 * Code adapted from Spencer Axiani's original software and designs by Jotham Gates
 * This file contains functions relating to the display
 * See MuonDetector.ino for more information
 */
#ifdef USE_SCREEN
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
#ifdef USE_SCREEN
void updateScreen() {
  //Only update the count and pulse size part of the display when required
  static unsigned long lastCount = 90; //Some value other than 0
  if (count != lastCount) {
    //unsigned long writeTime = micros(); //Check the screen update time. Around 10.5ms for this version, 32.0ms for the old hyphen graph
    lastCount = count;
    unsigned int sipmIntV = (unsigned int)(sipmVoltage); //Hopefully make processing faster.
    char charBuffer[16];
    ultoa(count, charBuffer, 10);
    u8x8.draw1x2String(6, 0, charBuffer);
    //Draw a line indicating the size of the pulse on the u8x8 display
    if (count == 0) {
      strcpy_P(charBuffer, detectorReady);
      u8x8.setInverseFont(1);
      u8x8.drawString(1, 4, charBuffer); //These overwrite the entire top line of the graph, so don't need to clear it beforehand.
      u8x8.setInverseFont(0);
    } else if (sipmIntV > THRESHOLD_LARGE) {
      strcpy_P(charBuffer, sipmWow);
      u8x8.setInverseFont(1);
      u8x8.drawString(1, 4, charBuffer);
      u8x8.setInverseFont(0);
    } else { //Bar graph part
      byte tempTile[120];
      memcpy_P(tempTile,barScaleT,120);
      //Serial.print(F("\tBAR TEST: Voltage F: ")); //Runs out of ram about here with this left in
      //Serial.print(sipmVoltage);
      //Serial.print(F(", Voltage I: "));
      //Serial.print(sipmIntV);
      //Number of pixels = V/180*119 sort of 120/180*V = V*6/9 = V*2/3
      unsigned int barLength = (sipmIntV * 2) / 3; //<<1 should be a fast multiply by 2 on an unsigned int
      //Serial.print(F(", Bar Length Px: "));
      //Serial.println(barLength);
      DEBUG_RAM();
      for(byte i = 2; i < barLength && i < 118; i++) { //Invert the bar graph to the rught length. LEaving the ends as they are white.
        tempTile[i] ^= CHAR_BAR_GRAPH; //Make the columns with pixels displaying the bar graph white.
      }
      u8x8.drawTile(1,4,15,tempTile);
      
      /*byte i = 0;
      for (; (i < (sipmVoltage + 10) / 10) && (i < 15); i++) {
        charBuffer[i] = CHAR_BAR_CHART;
      }
      //Fill the rest up with spaces to cover the previous graph
      for (; i < 15; i++) {
        charBuffer[i] = ' ';
      }
      //Null terminate the array
      charBuffer[i] = 0;*/
    }
    /*writeTime = micros() - writeTime;
    Serial.print(F("\tLCD Update time: "));
    Serial.println(writeTime);*/
  }
}
#endif

#ifdef USE_SCREEN
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
    decimalPlaces = 2;
    if (countStd < 10) {
      decimalPlaces = 3;
    }
    dtostrf(countStd, 5, decimalPlaces, charBuffer);
    u8x8.draw1x2String(11, 6, charBuffer);
    updateScreen();
    nextTime = millis();
    totalDeadtime += nextTime - timeOfUpdate;
    nextTime += SCREEN_UPDATE_INTERVAL;
  }
}
#endif
