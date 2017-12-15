const int RED_PIN = 9;
const int GREEN_PIN = 10;
const int BLUE_PIN = 11;
const int BTN_PIN = 7;

int DISPLAY_TIME = 50;  // In milliseconds, we like 50
unsigned long SHOW_DURATION = 1800000; //30 minutes
unsigned long startTime = 0;
bool isFinished = false;

void setup() {
  Serial.begin(9600);
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(BTN_PIN, INPUT);

  startTime = millis();
  Serial.println("Setup");
}


void loop() {
  int button = digitalRead(BTN_PIN);
  if (button == LOW) {
    Serial.println("BUTTON LOW");
    startTime = millis();
    isFinished = false;
  }

  if (isFinished) {
    return;
  }
  
  if (startTime + SHOW_DURATION >= millis()) {
    showSpectrum();
  } else {
    turnOffTheLights();
    isFinished = true;
  }
}

void turnOffTheLights() {
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GREEN_PIN, LOW);
  digitalWrite(BLUE_PIN, LOW);
}

void showSpectrum() {
  int x;  // define an integer variable called "x"
  for (x = 0; x < 768; x++) {
    showRGB(x);  // Call RGBspectrum() with our new x
    delay(DISPLAY_TIME);   // Delay for 10 ms (1/100th of a second) (we like 50)
  }
}

void showRGB(int color) {
  int redIntensity;
  int greenIntensity;
  int blueIntensity;

  if (color <= 255) {
    redIntensity = 255 - color;    // red goes from on to off
    greenIntensity = color;        // green goes from off to on
    blueIntensity = 0;             // blue is always off
  } else if (color <= 511) {
    redIntensity = 0;                     // red is always off
    greenIntensity = 255 - (color - 256); // green on to off
    blueIntensity = (color - 256);        // blue off to on
  } else {
    redIntensity = (color - 512);         // red off to on
    greenIntensity = 0;                   // green is always off
    blueIntensity = 255 - (color - 512);  // blue on to off
  }

  analogWrite(RED_PIN, redIntensity);
  analogWrite(BLUE_PIN, blueIntensity);
  analogWrite(GREEN_PIN, greenIntensity);
}
