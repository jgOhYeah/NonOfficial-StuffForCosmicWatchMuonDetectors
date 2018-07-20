//Settings to be saved to eeprom
//Device id - 2 parts for 2 lines. Concatenated for serial and files, separate lines of the first 16 chars for the screen.
//Max 40 chars each
#define detectorName1 "Muon " //Line 1 of the device id
#define detectorName2 "Detector" //Line 2 of the device id
//File format
#define saveTxtFiles //When left in, will save text files, when commented out, will save csv files.


//Stuff that probably shouldn't be changed - needs to match the values in the main program
#define deviceID1Address 0
#define deviceID2Address 40 //By using 40, first line should be the normal naming.ino setting
#define formatAddress 80
#define eepromStringLength 40

#define ledPin 3

#include <EEPROM.h>

void setup() {
  writeToEeprom(deviceID1Address,detectorName1);
  writeToEeprom(deviceID2Address,detectorName2);
#ifdef saveTxtFiles
  writeToEeprom(formatAddress,0);
#else
  writeToEeprom(formatAddress,1);
#endif

  pinMode(ledPin,OUTPUT);
  Serial.begin(9600);
  Serial.println(F("Written to eeprom"));
#define serialString1 "Detector ID 1: " detectorName1
#define serialString2 "Detector ID 2: " detectorName2
#ifdef saveTxtFiles
  #define serialString3 "File format: txt"
#else
  #define serialString3 "File format: csv"
#endif
  Serial.println(F(serialString1));
  Serial.println(F(serialString2));
  Serial.println(F(serialString3));
}

void loop() {
  digitalWrite(ledPin,HIGH);
  delay(1000);
  digitalWrite(ledPin,LOW);
  delay(1000);
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
