/*
  Project Name: Pomorodo Timer Shield
  Created: 2012-12-27
  By: John P. Reardon <jp@jpreardon.com>
  Version: 1.0.0 (2013-01-28)

  This is a simple timer shield that can be used to practice the Pomodoro Technique®. For
  a schematic and board layout visit this site http://jpreardon.com/projects/pomodoro-timer/
  
  For more on the Pomodoro Technique®, check out this site http://www.pomodorotechnique.com

  Copyright (c) 2012 John P. Reardon
  Licensed under the MIT license.
  
*/
#include <SoftwareSerial.h>

// Define debug state (1 for debug, 0 for not)
#define DEBUG 1

// Define available modes
#define IDLE 1
#define SHORT_BREAK 2
#define LONG_BREAK 3
#define POMODORO 4
#define IN_PROCESS 5

// Constants for input/output pins
const int greenLed1 = 8;   // Green LEDs are for the pomodoro count
const int greenLed2 = 7;
const int greenLed3 = 4;
const int greenLed4 = 2;  
const int redLed1 = 6;     // Red LEDs count the time left
const int blueLed1 = 10;    // Short break indicator
const int blueLed2 = 3;    // Long break indicator
const int button1 = 11;    // Start/Interrupt
const int button2 = 12;    // Reset

//Buzzer
const int buzzer = 5;
const int buzzerTone = 262;
const int completePomodoroTime = 1500;
const int completeBreakTime = 500;

// Constants for time values (in minutes)
const int pomodoroTime = 25;  // Should be 25
const int shortBreakTime = 5; // Should be 5
const int longBreakTime = 15; // Should be 15

// Fields to hold state
int currentMode = IDLE;    // Values are IDLE, SHORT_BREAK, LONG_BREAK, POMODORO, IN_PROCESS
int nextMode = POMODORO;   // Holds the next mode
int currentPomodoroCount;  // Number of completed pomodoros
long stateStartTime;       // In milliseconds
int modeFinished = 0;      // To let us know if the last pomodoro or break was completed

// LED Display variables
// Blinky
int ledState = LOW;         // This will be used if a LED needs to blink
int blinkInterval = 1000;   // In milliseconds
long previousMillis = 0;    // Last time the LED was updated

//Fade
int ledFadeAmount = 1;                   // The amount to fade the LED(s)
int minLedBrightness = 10;               // The minimum value for fading
int maxLedBrightness = 255;              // Maximum brightness value for lit LEDs
int maxLedFadeBrightness = 200;          // Fading LEDs are a little dimmer
int ledBrightness = minLedBrightness;    // The current brightness of the LED(s)
int fadeInterval = 1500;                 // In milliseconds, the amount of time for fade cycle
long previousFadeMillis = 0;             // Last time the fade LED was updated

//s7s
const int softwareTx = 13;
const int softwareRx = 0;
SoftwareSerial s7s(softwareRx, softwareTx);
char tempString[10];
char tempStringM[10];

void setup() {
  if (DEBUG) Serial.begin(9600);
  
  // Set pins to the proper mode
  pinMode(greenLed1, OUTPUT);
  pinMode(greenLed2, OUTPUT);
  pinMode(greenLed3, OUTPUT);
  pinMode(greenLed4, OUTPUT);
  pinMode(redLed1, OUTPUT);
  pinMode(blueLed1, OUTPUT);
  pinMode(blueLed2, OUTPUT);
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);

  // Clear the display, and then turn on all segments and decimals
  s7s.begin(9600);  
  clearDisplay();  // Clears display, resets cursor
  s7s.print("PODO");  // Displays -HI- on all digits
  setDecimals(0b00010000);  // Turn on all decimals, colon, apos
  setBrightness(255);  // High brightness
  
}

void loop() {
  // Check to see if buttons are pressed
  checkButtons();
  
  // Increment Everything
  incrementTime();
  
  // Display current state
  displayState();

  displayTime();
}

// Check the buttons to see if one of them is pressed
void checkButtons() {
  // See if any button are pressed, if they are, do something
  
  if(digitalRead(button2) == LOW) {
    // debounce, ignore any button presses one second after a state change
    if ((millis() - stateStartTime) < 1000) {
      return;
    }
    // full reset
    setCurrentMode(IDLE);
    modeFinished = 0;
    resetPomodoroCount();
    if (DEBUG) Serial.println("RESET!");
  }
  
  if(digitalRead(button1) == LOW) {
    // debounce, ignore any button presses one second after a state change
    if ((millis() - stateStartTime) < 1000) {
      return;
    }
    clearDisplay();
    switch (currentMode) {
      case IDLE:
        resetPomodoroCount();
        modeFinished = 0;
        setCurrentMode(POMODORO);
        nextMode = SHORT_BREAK;
        if (DEBUG) Serial.println("changing to pomodoro mode");
        break;
      case SHORT_BREAK:
        setCurrentMode(IN_PROCESS);
        nextMode = POMODORO;
        if (DEBUG) Serial.println("changing to in process mode from short break");
        break;
      case LONG_BREAK:
        resetPomodoroCount();
        setCurrentMode(IDLE);
        if (DEBUG) Serial.println("changing to idle mode from long break");
        break;
      case POMODORO:
        setCurrentMode(IN_PROCESS);
        nextMode = POMODORO;  
        if (DEBUG) Serial.println("changing to in process mode from pomodoro");
        break;
      case IN_PROCESS:
        setCurrentMode(nextMode);
        modeFinished = 0;
        if (DEBUG) Serial.print("changing from in process to next mode: ");
        if (DEBUG) Serial.println(nextMode);
        break;
    }
  }
}

// This is the function that will set the proper state when the time has elapsed
// for a given mode.
void incrementTime() {
  
  unsigned long currentMillis = millis();
  
  // Increment the timer, see if the state needs to change
  switch (currentMode) {
    case IDLE:
      // Idle mode, do nothing
      break;
    case SHORT_BREAK:
      // short break code here
      // If break is over, change modes
      if(currentMillis - stateStartTime > convertMinuteToMillis(shortBreakTime)){
        setCurrentMode(IN_PROCESS);        
        tone(buzzer,buzzerTone,completeBreakTime);
        nextMode = POMODORO;
        modeFinished = 1;
      }
      break;
    case LONG_BREAK:
      // long break code here
      if(currentMillis - stateStartTime > convertMinuteToMillis(longBreakTime)){
        setCurrentMode(IDLE);        
        tone(buzzer,buzzerTone,completeBreakTime);        
        nextMode = POMODORO;
        modeFinished = 1;
      }
      break;
    case POMODORO:
      // pomodoro code here
      if(currentMillis - stateStartTime > convertMinuteToMillis(pomodoroTime)){
        currentPomodoroCount++;
        modeFinished = 1;        
        tone(buzzer,buzzerTone,completePomodoroTime);             
        if (currentPomodoroCount == 4) {
          setCurrentMode(LONG_BREAK);
          nextMode = LONG_BREAK;
        } else {
          setCurrentMode(SHORT_BREAK);
          nextMode = SHORT_BREAK;
        }        
      }
      
      break;
    case IN_PROCESS:
      // do nothing
      
    default:
      // do nothing
      break;
  }
 
}

// This function displays the current state
void displayState() {
  
  unsigned long currentMillis = millis();
  unsigned long millisPassed;
  float numLeds;
  
  // We always light the proper number of pomodoro number lights, unless the state is idle
  if (currentMode != IDLE) {
    lightGreenLeds(currentPomodoroCount);
  }

  // Light up leds based on the current state
  switch (currentMode) {
    case IDLE:
      // If we reached the end of the pomodoro cycle, blink the green leds
      if (modeFinished) {
        blinkGreenLeds();
      } 
      else {
        turnAllOff();
      }
      break;
    case SHORT_BREAK:
      // We're on a break, light a break light
      digitalWrite(blueLed1, HIGH);
      // Figure out how much time has passed
      millisPassed = currentMillis - stateStartTime;
      // Figure out how many LEDs to light up, cast a variable as a float so we get a float back
      numLeds = ((float)shortBreakTime - convertMillisToMinute(millisPassed)) / (shortBreakTime / 5); 
        
      // Light em up
      lightRedLeds();
      break;
    case LONG_BREAK:
      // We're on a break, light a break light
      digitalWrite(blueLed2, HIGH);
      // Figure out how much time has passed
      millisPassed = currentMillis - stateStartTime;
      // Figure out how many LEDs to light up, cast a variable as a float so we get a float back
      numLeds = ((float)longBreakTime - convertMillisToMinute(millisPassed)) / (longBreakTime / 5);
      
      // Light em up
      lightRedLeds();
      break;
    case POMODORO:
      // Turn off the break lights
      digitalWrite(blueLed1, LOW);
      digitalWrite(blueLed2, LOW);      
      lightRedLeds();
      break;
      
    case IN_PROCESS:
      if (modeFinished) {
        blinkRedLeds();
      }
      else {
        analogWrite(redLed1, LOW);
      }
      break;
    default:
      // Do nothing
      break;
  }
  
}

// Blink the red LEDs
void blinkRedLeds() {
  digitalWrite(redLed1, blinkLed());
}

// Blink the green LEDs
void blinkGreenLeds() {
  digitalWrite(greenLed1, blinkLed());
  digitalWrite(greenLed2, blinkLed());
  digitalWrite(greenLed3, blinkLed());
  digitalWrite(greenLed4, blinkLed());
}

// Light the Pomodor LED
// With a nice fade
void lightRedLeds() {
  analogWrite(redLed1, fadeLed());
}

// Light the number of green LEDs specified
void lightGreenLeds(int numLeds) {
    switch (numLeds) {
    case 1:
      digitalWrite(greenLed1, HIGH);
      digitalWrite(greenLed2, LOW);
      digitalWrite(greenLed3, LOW);
      digitalWrite(greenLed4, LOW);
      break;
    case 2:
      digitalWrite(greenLed1, HIGH);
      digitalWrite(greenLed2, HIGH);
      digitalWrite(greenLed3, LOW);
      digitalWrite(greenLed4, LOW);
      break;
    case 3:
      digitalWrite(greenLed1, HIGH);
      digitalWrite(greenLed2, HIGH);
      digitalWrite(greenLed3, HIGH);
      digitalWrite(greenLed4, LOW);
      break;
    case 4:
      digitalWrite(greenLed1, HIGH);
      digitalWrite(greenLed2, HIGH);
      digitalWrite(greenLed3, HIGH);
      digitalWrite(greenLed4, HIGH);
      break;
    default:
      digitalWrite(greenLed1, LOW);
      digitalWrite(greenLed2, LOW);
      digitalWrite(greenLed3, LOW);
      digitalWrite(greenLed4, LOW);
  }
}
 
// Should a blinky LED be lit or not? Returns a HIGH or LOW value based on time
int blinkLed() {
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis > blinkInterval) {
    
    previousMillis = currentMillis;
    
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
  }
  return ledState;
}

// Returns a fade value
int fadeLed() {
  unsigned long currentMillis = millis();
  
  if ((currentMillis - previousFadeMillis) > fadeInterval / ((maxLedFadeBrightness - minLedBrightness) / abs(ledFadeAmount))) {
    ledBrightness = ledBrightness + ledFadeAmount;
    previousFadeMillis = currentMillis;
    if (ledBrightness == minLedBrightness || ledBrightness == maxLedFadeBrightness) {
      ledFadeAmount = -ledFadeAmount ; 
    } 
  }
  return ledBrightness;
}
 
void turnAllOn() {
 // You guessed it, turn all the lights on at once
 digitalWrite(greenLed1, HIGH);
 digitalWrite(greenLed2, HIGH);
 digitalWrite(greenLed3, HIGH);
 digitalWrite(greenLed4, HIGH);
 digitalWrite(redLed1, HIGH);
 digitalWrite(blueLed1, HIGH);
 digitalWrite(blueLed2, HIGH);
}

void turnAllOff() {
 // You guessed it, turn all the lights off at once
 digitalWrite(greenLed1, LOW);
 digitalWrite(greenLed2, LOW);
 digitalWrite(greenLed3, LOW);
 digitalWrite(greenLed4, LOW);
 digitalWrite(redLed1, LOW);
 digitalWrite(blueLed1, LOW);
 digitalWrite(blueLed2, LOW);
}

long convertMinuteToMillis(int minutes) {
  return minutes * 60000;
}

int convertMillisToMinute(long millis) {
  return (millis / 60000);
}

int convertMillisToSeconds(long millis) {
  return (millis / 1000);
}

// This sets the current mode
void setCurrentMode(int newMode) {
  currentMode = newMode;
  stateStartTime = millis();
}

void resetPomodoroCount() {
  currentPomodoroCount = 0;
}

void displayTime() {
  unsigned long currentMillis = millis();
  unsigned long millisPassed;

  // Figure out how much time has passed
  millisPassed = currentMillis - stateStartTime;
  int seconds = convertMillisToSeconds(millisPassed);
  int STm = (seconds / 60) % 60;
  int STs = seconds % 60;

  //Formant the String
  sprintf(tempString, "%.2d%.2d", STm, STs);
  setDecimals(0b00010000);
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

