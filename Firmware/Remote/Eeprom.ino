#include <Wire.h>

#define DEVICEADDRESS   0x50

void setupEeprom() {
  Wire.begin();
}

const uint16_t memoryAddresses[] = {0, 6, 12, 18, 24};

void storeCodeInEeprom(int button, int codeType, int codeLength, unsigned long codeValue) {
    uint16_t memoryAddress = memoryAddresses[button];
    wireEepromWrite(memoryAddress, codeType);
    wireEepromWrite(memoryAddress + 1, codeLength);
    storeCodeValue(memoryAddress + 2, codeValue);
}

void storeCodeValue(uint16_t memoryAddress, unsigned long codeValue) {
  uint8_t first = codeValue & 0xFF;
  uint8_t second = (codeValue >> 8) & 0xFF;
  uint8_t third = (codeValue >> 16) & 0xFF;
  uint8_t fourth = (codeValue >> 24) & 0xFF;
  
  wireEepromWrite(memoryAddress, first);
  wireEepromWrite(memoryAddress + 1, second); 
  wireEepromWrite(memoryAddress + 2, third); 
  wireEepromWrite(memoryAddress + 3, fourth); 
}

int readCodeTypeFromEeprom(int button) {
  uint16_t memoryAddress = memoryAddresses[button];
  return wireEepromRead(memoryAddress);
}

int readCodeLengthFromEeprom(int button) {
  uint16_t memoryAddress = memoryAddresses[button] + 1;
  return wireEepromRead(memoryAddress);
}

unsigned long readCodeValueFromEeprom(int button) {
  uint16_t memoryAddress = memoryAddresses[button] + 2;
  
  unsigned long first = wireEepromRead(memoryAddress);
  unsigned long second = wireEepromRead(memoryAddress + 1);
  unsigned long third = wireEepromRead(memoryAddress + 2);
  unsigned long fourth = wireEepromRead(memoryAddress + 3);
  
  unsigned long codeValue = first + (second << 8) + (third << 16) + (fourth << 24);
  return codeValue;
}

void wireEepromWrite(uint16_t theMemoryAddress, uint8_t u8Byte) {
    Wire.beginTransmission(DEVICEADDRESS);
    Wire.write( (theMemoryAddress >> 8) & 0xFF );
    Wire.write( (theMemoryAddress >> 0) & 0xFF );
    Wire.write(u8Byte);
    Wire.endTransmission();
    delay(5);
}

uint8_t wireEepromRead(uint16_t theMemoryAddress) {
  uint8_t u8retVal = 0;
  Wire.beginTransmission(DEVICEADDRESS);
  Wire.write( (theMemoryAddress >> 8) & 0xFF );
  Wire.write( (theMemoryAddress >> 0) & 0xFF );
  Wire.endTransmission();
  delay(5);
  Wire.requestFrom(DEVICEADDRESS, 1);
  u8retVal = Wire.read();
  return u8retVal;
}
