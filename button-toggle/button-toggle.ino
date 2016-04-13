//Plus Button
int inPin = 2;         // the number of the input pin
int outPin = 13;       // the number of the output pin
int state = HIGH;      // the current state of the output pin
int reading;           // the current reading from the input pin
int previous = LOW;    // the previous reading from the input pin

// the follow variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long time = 0;         // the last time the output pin was toggled
long debounce = 200;   // the debounce time, increase if the output flickers

// this constant won't change:
const int  buttonPin = 3;    // the pin that the pushbutton is attached to
const int ledPin = 12;       // the pin that the LED is attached to

// Variables will change:
int buttonPushCounter = 0;   // counter for the number of button presses
int buttonState = 0;         // current state of the button
int lastButtonState = 0;     // previous state of the button

void setup() {
  //buttons
  pinMode(inPin, INPUT);
  pinMode(buttonPin, INPUT);
  
  //LEDs
  pinMode(outPin, OUTPUT);
  pinMode(ledPin, OUTPUT);

  //Serial
  Serial.begin(9600);
}

void loop() {
  //Plush Button (Green Button)
  reading = digitalRead(inPin);
//  if (reading == HIGH && previous == LOW && millis() - time > debounce) {  
//    if (state == HIGH) {
//      state = LOW;
//    } else {
//      state = HIGH;
//    }    
//    time = millis();
//  }
//  digitalWrite(outPin, state);

  if (reading != previous) {
    if (reading == HIGH) {
      buttonPushCounter++;
      Serial.print("number of button pushes:  ");
      Serial.println(buttonPushCounter);      

    }
    delay(50);
  }

  if (reading == LOW) {
    digitalWrite(outPin, HIGH);
  } else {
    digitalWrite(outPin, LOW);
  }
  previous = reading;

  //Minus Button (red)   
  buttonState = digitalRead(buttonPin);
  if (buttonState != lastButtonState) {
    if (buttonState == HIGH) {
      buttonPushCounter--;
      Serial.print("number of button pushes:  ");
      Serial.println(buttonPushCounter);      
    }
    delay(50);
  }
  
  if(buttonState == LOW) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }
    
  lastButtonState = buttonState;
}

