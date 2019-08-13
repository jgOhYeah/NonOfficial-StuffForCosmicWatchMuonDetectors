/*other.ino
 * Code adapted from Spencer Axiani's original software and designs by Jotham Gates
 * This file contains general other functions.
 * See MuonDetector.ino for more information
 */
// This function converts the measured ADC value to a SiPM voltage via the calibration array
float get_sipm_voltage(float adc_value) {
  float voltage = 0;
  for (int i = 0; i < (sizeof(cal) / sizeof(float)); i++) {
    voltage += cal[i] * pow(adc_value, (sizeof(cal) / sizeof(float) - i - 1));
  }
  return voltage;
}
#ifdef USE_SERIAL_DEBUG
//A function from https://learn.adafruit.com/memories-of-an-arduino/measuring-free-memory to get the space between the stack and heap in RAM to help with debugging running out of memory
#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__
 
int freeRam() {
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else  // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}
void debugRam(unsigned int lineNumber) {
  Serial.print(F("Line "));
  Serial.print(lineNumber);
  Serial.print(F(" RAM Left: "));
  Serial.println(freeRam());
}
#endif
