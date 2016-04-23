#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <Phant.h>

// Wifi
const char WiFiSSID[] = "4334";
const char WiFiPSK[] = "clubdv88";

// Phant Keys
const char PhantHost[] = "data.sparkfun.com";
const char PublicKey[] = "EJGqnJw13wHO3RXAYnxz";
const char PrivateKey[] = "dqWdDqZG1ZUXZKnBVbWx";


//Pins
const int LED_PIN = 5; // onboard, green LED
const int LED_RED = 4; // red
#define PIN_ANALOG_IN A0

// Config
const unsigned long postRate = 60000;
unsigned long lastPost = 0;
int pass = 0;


void setup() {
  Serial.begin(9600);
  Serial.println("Set Up");
  
  connectWiFi(); // Connect to WiFi
  turnOffLEDs();
  
  pinMode(LED_RED, OUTPUT);

 // Display status
  Serial.println("Initialized");
}

void loop() {
  soundCheck();
  delay(1000);
}

void soundCheck() {
  int value;

  // Check the envelope input
  value = analogRead(PIN_ANALOG_IN);
  Serial.println(value);
  // Convert envelope value into a message
  Serial.print("Status: ");
  if(value <= 10)
  {
    Serial.println("Quiet.");
  }
  else if( (value > 10) && ( value <= 30) )
  {
    Serial.println("Moderate.");
  }
  else if(value > 30)
  {
    Serial.println("Loud.");
  }
}
void turnOnLEDs() {
  digitalWrite(LED_PIN, LOW);
  digitalWrite(LED_RED, LOW);
//  digitalWrite(LED_BLU, LOW);
}

void turnOffLEDs() {
  digitalWrite(LED_PIN, HIGH);
  digitalWrite(LED_RED, HIGH);
//  digitalWrite(LED_BLU, HIGH);
}
void connectWiFi() {
  byte ledStatus = LOW;
  Serial.println();
  Serial.println("Connecting to: " + String(WiFiSSID));
  // Set WiFi mode to station (as opposed to AP or AP_STA)
  WiFi.mode(WIFI_STA);

  // WiFI.begin([ssid], [passkey]) initiates a WiFI connection
  // to the stated [ssid], using the [passkey] as a WPA, WPA2,
  // or WEP passphrase.
  WiFi.begin(WiFiSSID, WiFiPSK);

  // Use the WiFi.status() function to check if the ESP8266
  // is connected to a WiFi network.
  while (WiFi.status() != WL_CONNECTED) {
    // Blink the LED
    digitalWrite(LED_PIN, ledStatus); // Write LED high/low
    ledStatus = (ledStatus == HIGH) ? LOW : HIGH;

    // Delays allow the ESP8266 to perform critical tasks
    // defined outside of the sketch. These tasks include
    // setting up, and maintaining, a WiFi connection.
    delay(100);
    
  }
  
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


