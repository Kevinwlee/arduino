const int plusInPin = 2;
const int minusInPin = 3;
const int plusOutPin = 12;
const int minusOutPin = 11;

int plusReading = LOW;
int plusPrevious = LOW;
int minusReading = LOW;
int minusPrevious = LOW;

int tally = 0;   // tally

//Ligthing
int ledPin = 13;
int sensorPin = 0;

void setup() {
  //buttons
  pinMode(plusInPin, INPUT);
  pinMode(minusInPin, INPUT);
  
  //LEDs
  pinMode(plusOutPin, OUTPUT);
  pinMode(minusOutPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  
  //Serial
  Serial.begin(9600);
}

void loop() {
  handleLighting();
  handlePlus();
  handleMinus();
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
  
  if (plusReading == LOW) {
    digitalWrite(plusOutPin, HIGH);
  } else {
    digitalWrite(plusOutPin, LOW);
  }
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

  if(minusReading == LOW) {
    digitalWrite(minusOutPin, HIGH);
  } else {
    digitalWrite(minusOutPin, LOW);
  }    
}

void handleLighting() {
  int sensorValue = analogRead(sensorPin);
  if (sensorValue > 500) {
    digitalWrite(ledPin, LOW);
  } else {
    digitalWrite(ledPin, HIGH);
  }
}
