#include <SoftwareSerial.h>

const int firstInPin = 2;
const int secondInPin = 3;
const int thirdInPin = 4;
const int funInPin = 5;

const int firstOutPin = 12;
const int secondOutPin = 11;
const int thirdOutPin = 10;
const int funOutPin = 9;

int firstReading = LOW;
int firstPrevious = LOW;
int secondReading = LOW;
int secondPrevious = LOW;
int thirdReading = LOW;
int thirdPrevious = LOW;
int funReading = LOW;
int funPrevious = LOW;

long stateStartTime;       // In milliseconds
long firstPausedTime;
long funPausedTime;

//s7s
const int softwareTx = 8;
const int softwareRx = 7;
SoftwareSerial s7s(softwareRx, softwareTx);
char tempString[10];

void setup() {  
  
  //buttons
  pinMode(firstInPin, INPUT);
  pinMode(secondInPin, INPUT);
  pinMode(thirdInPin, INPUT);
  pinMode(funInPin, INPUT);
  
  //LEDs
  pinMode(firstOutPin, OUTPUT);
  pinMode(secondOutPin, OUTPUT);
  pinMode(thirdOutPin, OUTPUT);
  pinMode(funOutPin, OUTPUT);
  
  //Serial
  Serial.begin(9600);
  s7s.begin(9600);

  clearDisplay();
  s7s.print("-HI-");
  setDecimals(0b111111);  // Turn on all decimals, colon, apos

  setBrightness(0);  // Lowest brightness
  delay(500);
  setBrightness(127);  // Medium brightness
  delay(500);
  setBrightness(255);  // High brightness
  delay(500);

  // Clear the display before jumping into loop
  clearDisplay();
  setDecimals(0b00010000);
  stateStartTime = millis();
}

void loop() {
  handleFirst();
  handleSecond();
  handleThird();
  handleFun();
  writeToDisplay();
  
}

void turnOffLeds() {
  digitalWrite(firstOutPin, LOW);
  digitalWrite(secondOutPin, LOW);
  digitalWrite(thirdOutPin, LOW);
  digitalWrite(funOutPin, LOW);
}

void handleFirst() {
  firstReading = digitalRead(firstInPin);
  if (firstReading != firstPrevious) {
    if (firstReading == HIGH) {      
      turnOffLeds();
      digitalWrite(firstOutPin, HIGH);
      //puase other timers
      //start this timer      
    }
    delay(50);
  }
  firstPrevious = firstReading;  
}

void handleSecond() {
  secondReading = digitalRead(secondInPin);
  if (secondReading != secondPrevious) {
    if (secondReading == HIGH) {      
      turnOffLeds();
      digitalWrite(secondOutPin, HIGH);
      //puase other timers
      //start this timer
    }
    delay(50);
  }
  secondPrevious = secondReading;
}

void handleThird() {
  thirdReading = digitalRead(thirdInPin);
  if (thirdReading != thirdPrevious) {
    if (thirdReading == HIGH) {
      turnOffLeds();
      digitalWrite(thirdOutPin, HIGH);
      //puase other timers
      //start this timer      
    }
    delay(50);
  }
  thirdPrevious = thirdReading;
}

void handleFun() {
  funReading = digitalRead(funInPin);
  if (funReading != funPrevious) {
    if (funReading == HIGH) {
      turnOffLeds();
      digitalWrite(funOutPin, HIGH);
      //puase other timers
      //start this timer      
    }
    delay(50);
  }
  funPrevious = funReading;
}

//s7 display
void writeToDisplay() {
  unsigned long currentMillis = millis();
  unsigned long millisPassed;
  millisPassed = currentMillis - stateStartTime;
  int seconds = millisPassed/1000;
  int STh = (seconds / 60 / 60) % 60;  
  int STm = (seconds / 60) % 60;
  int STs = seconds % 60;

  //Formant the String
  if (seconds > 3600000) {
    sprintf(tempString, "%.2d%.2d", STh, STm);
  } else {
    sprintf(tempString, "%.2d%.2d", STm, STs);
  }  
  s7s.print(tempString);
}

void clearDisplay() {
  s7s.write(0x76);  // Clear display command
}

// Set the displays brightness. Should receive byte with the value
//  to set the brightness to
//  dimmest------------->brightest
//     0--------127--------255
void setBrightness(byte value) {
  s7s.write(0x7A);  // Set brightness command byte
  s7s.write(value);  // brightness data byte
}

// Turn on any, none, or all of the decimals.
//  The six lowest bits in the decimals parameter sets a decimal 
//  (or colon, or apostrophe) on or off. A 1 indicates on, 0 off.
//  [MSB] (X)(X)(Apos)(Colon)(Digit 4)(Digit 3)(Digit2)(Digit1)
void setDecimals(byte decimals) {
  s7s.write(0x77);
  s7s.write(decimals);
}
