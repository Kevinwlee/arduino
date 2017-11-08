#include <stdint.h>
#include "SparkFunBME280.h"

#include "Wire.h"
//#include "SPI.h"

//Global sensor object
BME280 mySensor;

void setup() {
  Serial.begin(9600);
  Serial.println("BME280 test!");
  
    mySensor.settings.commInterface = I2C_MODE;
    mySensor.settings.I2CAddress = 0x77;
    mySensor.settings.runMode = 3; //Forced mode
    mySensor.settings.tStandby = 0;
    mySensor.settings.filter = 0;
    mySensor.settings.tempOverSample = 1;
    mySensor.settings.pressOverSample = 1;
    mySensor.settings.humidOverSample = 1;
    
    delay(10);  //Make sure sensor had enough time to turn on. BME280 requires 2ms to start up.
    Serial.print("Starting BME280... result of .begin(): 0x");
    
    //Calling .begin() causes the settings to be loaded
    Serial.println(mySensor.begin(), HEX);

}

void loop () {
  float h = mySensor.readFloatHumidity();
  float f = mySensor.readTempF();
  float c = mySensor.readTempC();
  float p = mySensor.readFloatPressure();
  float m = mySensor.readFloatAltitudeMeters();
  float ft = mySensor.readFloatAltitudeFeet();
  float inches = (p * 0.0004);
  
  Serial.print("Humidity: ");
  Serial.println(h);
  
  Serial.print("Temp:  ");
  Serial.println(f);
  
  Serial.print("Temp C:  ");
  Serial.println(c);
  
  Serial.print("Pressure: ");
  Serial.println(p);
  Serial.println(inches);
  
  Serial.print("Altitude Meters: ");
  Serial.println(m);

  Serial.print("Altitude Feet:  ");
  Serial.println(ft);
  
  delay(5000);
}

