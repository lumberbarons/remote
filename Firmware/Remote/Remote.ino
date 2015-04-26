#include <IRremote.h>

const int BUTTON_PIN_1 = A0;
const int BUTTON_PIN_2 = A1;
const int BUTTON_PIN_3 = A2;
const int BUTTON_PIN_4 = A3;
const int BUTTON_PIN_5 = 2;

const int buttonCount = 5;
const int buttonPins[] = { BUTTON_PIN_1, BUTTON_PIN_2, BUTTON_PIN_3, BUTTON_PIN_4, BUTTON_PIN_5 };

const int STATUS_LED_1 = 6;
const int STATUS_LED_2 = 7;

const int RECV_PIN = 11;

const int BUTTON_TX = 0;
const int BUTTON_RX = 1;

// button debouncing
long lastDebounceTime[buttonCount];
const long debounceDelay = 50;

int currentButtonState[buttonCount];
int lastButtonState[buttonCount];

int buttonState[buttonCount];
int buttonToggle[buttonCount];

// hold button
int buttonHoldStart[buttonCount];
const int holdTime = 5000; // 5 seconds
int holdButton = -1;

// ir tx/rx
IRrecv irrecv(RECV_PIN);
IRsend irsend;

decode_results results;

int codeCount = 0;
int codeTypes[5];
int codeLengths[5];
unsigned long codeValues[5];

void setup() {   
  Serial.begin(9600);
  
  irrecv.enableIRIn();
  setupEeprom();
  
  for(int x = 0; x < buttonCount; x++) {
    int pin = buttonPins[x];
    pinMode(pin, INPUT);
  }
  
  pinMode(STATUS_LED_1, OUTPUT);
  pinMode(STATUS_LED_2, OUTPUT);
  
  digitalWrite(STATUS_LED_1, HIGH);
  delay(250);
  digitalWrite(STATUS_LED_1, LOW);
}

void getButtonStates() {
  for(int x = 0; x < buttonCount; x++) {
    buttonToggle[x] = LOW;
    
    int pin = buttonPins[x];
    currentButtonState[x] = digitalRead(pin);
    
    if (currentButtonState[x] != lastButtonState[x]) {
      lastDebounceTime[x] = millis();
    }
    
    if ((millis() - lastDebounceTime[x]) > debounceDelay) {   
      if(currentButtonState[x] != buttonState[x]) {
        buttonState[x] = currentButtonState[x];
        buttonToggle[x] = HIGH;
        Serial.print("BTN ");
        Serial.println(x, DEC);
      }
    }
    
    lastButtonState[x] = currentButtonState[x];
  }
}

void checkForButtonHolds() {
  if(holdButton > -1) {
    return;
  }
  
  for(int x = 0; x < buttonCount; x++) {
    if(buttonState[x]) {      
      if(buttonToggle[x]) {
        buttonHoldStart[x] = millis();
      } else {
        int elapsed = millis() - buttonHoldStart[x];
        if(elapsed > holdTime) {
          holdButton = x;
          codeCount = 0;
          digitalWrite(STATUS_LED_2, HIGH);
        }
      }
    } else {
     buttonHoldStart[x] = 0;
    }
  }
}

void storeCode(int button, decode_results *results) {
  int codeType = results->decode_type;  
  
  if (codeType == UNKNOWN) {
    return;
  }
  
  if(codeType == NEC && results->value == REPEAT) {
    return;
  }

  printCode(codeType, results->bits, results->value);
  
  codeTypes[codeCount] = codeType;
  codeLengths[codeCount] = results->bits;
  codeValues[codeCount] = results->value;
  
  codeCount++;
}

void printCode(int codeType, int codeLength, unsigned long codeValue) {
  Serial.print("#");
  Serial.print(codeCount, DEC);
  Serial.print(", ");
  Serial.print(codeType, HEX);
  Serial.print(", ");
  Serial.print(codeLength, HEX);
  Serial.print(", ");
  Serial.println(codeValue, HEX);
}

void sendCode(int button) {
  int codeType = readCodeTypeFromEeprom(button);
  int codeLen = readCodeLengthFromEeprom(button);
  unsigned long codeValue = readCodeValueFromEeprom(button);
  if (codeType == NEC) {
    irsend.sendNEC(codeValue, codeLen); 
  } 
}

void maybeReadCodes() {
  if(buttonState[holdButton] && buttonToggle[holdButton]) {
    Serial.println("CLEAR READ");
    holdButton = -1;
    digitalWrite(STATUS_LED_2, LOW);
  } else if(irrecv.decode(&results)) {
    digitalWrite(STATUS_LED_1, HIGH);
    storeCode(holdButton, &results);
    irrecv.resume();
    digitalWrite(STATUS_LED_1, LOW);
  }
}

void maybeSendCodes() {  
  if(irrecv.decode(&results)) {
    irrecv.resume(); // ignore
  }
  
  for(int x = 0; x < buttonCount; x++) {
    if(buttonState[x] && buttonToggle[x]) {
      digitalWrite(STATUS_LED_1, HIGH);
      sendCode(x);
      digitalWrite(STATUS_LED_1, LOW);
      irrecv.enableIRIn();
    }
  }
}

void loop() {
  getButtonStates();
  checkForButtonHolds();
  
  if(holdButton > -1) {
    maybeReadCodes();
  } else {
    maybeSendCodes();
  }
}
