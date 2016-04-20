#include <SoftwareSerial.h>

const int plusInPin = 0;
const int minusInPin = 2;
const int resetInPin = 4;

int plusReading = LOW;
int plusPrevious = LOW;
int minusReading = LOW;
int minusPrevious = LOW;
int resetReading = LOW;
int resetPrevious = LOW;

//s7s
const int softwareTx = 12; //to s7s RX
const int softwareRx = 7;
SoftwareSerial s7s(softwareRx, softwareTx);
char tempString[10];

//tally
int tally = 0;

void setup() {
  //buttons
  pinMode(plusInPin, INPUT);
  pinMode(minusInPin, INPUT);
  pinMode(resetInPin, INPUT);
  
  //Serial
  Serial.begin(9600);
  
  // Must begin s7s software serial at the correct baud rate.
  //  The default of the s7s is 9600.
  s7s.begin(9600);

  // Clear the display, and then turn on all segments and decimals
  clearDisplay();  // Clears display, resets cursor
  s7s.print("-HI-");  // Displays -HI- on all digits
  setDecimals(0b111111);  // Turn on all decimals, colon, apos

  // Flash brightness values at the beginning
  setBrightness(0);  // Lowest brightness
  delay(500);
  setBrightness(127);  // Medium brightness
  delay(500);
  setBrightness(255);  // High brightness
  delay(500);

  // Clear the display before jumping into loop
  clearDisplay();
}

void loop() {
  handlePlus();
  handleMinus();
  handleReset();
  writeToDisplay();
}

void handlePlus() {
  plusReading = digitalRead(plusInPin);
  if (plusReading != plusPrevious) {
    if (plusReading == HIGH) {
      tally++;
      Serial.print("number of button pushes:  ");
      Serial.println(tally);      
    }
    delay(50);
  }
  plusPrevious = plusReading;
  
}

void handleMinus() {
  minusReading = digitalRead(minusInPin);
  if (minusReading != minusPrevious) {
    if (minusReading == HIGH) {
      tally--;
      Serial.print("number of button pushes:  ");
      Serial.println(tally);      
    }
    delay(50);
  }
  minusPrevious = minusReading; 
}

void handleReset() {
  resetReading = digitalRead(resetInPin);
  if (resetReading != resetPrevious) {
    if (resetReading == HIGH) {
      tally = 0;
      Serial.print("number of button pushes:  ");
      Serial.println(tally);      
    }
    delay(50);
  }
  resetPrevious = resetReading;
}

void writeToDisplay() {
  // Magical sprintf creates a string for us to send to the s7s.
  //  The %4d option creates a 4-digit integer.
  sprintf(tempString, "%4d", tally);

  // This will output the tempString to the S7S
  s7s.print(tempString);
}

void clearDisplay()
{
  s7s.write(0x76);  // Clear display command
}

// Set the displays brightness. Should receive byte with the value
//  to set the brightness to
//  dimmest------------->brightest
//     0--------127--------255
void setBrightness(byte value)
{
  s7s.write(0x7A);  // Set brightness command byte
  s7s.write(value);  // brightness data byte
}

// Turn on any, none, or all of the decimals.
//  The six lowest bits in the decimals parameter sets a decimal 
//  (or colon, or apostrophe) on or off. A 1 indicates on, 0 off.
//  [MSB] (X)(X)(Apos)(Colon)(Digit 4)(Digit 3)(Digit2)(Digit1)
void setDecimals(byte decimals)
{
  s7s.write(0x77);
  s7s.write(decimals);
}
