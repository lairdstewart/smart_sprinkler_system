//following Controller Reader from: https://docs.arduino.cc/learn/communication/wire
#include <Wire.h>

bool debug = 1; 

void setup() {
  Wire.begin(); // join i2c bus
  Serial.begin(9600); // serial output to console
}

void select_sensor(int sensor_index) {
  Wire.beginTransmission(8); 
  Wire.write(sensor_index); // one byte 
  Wire.endTransmission(); 
}

float request_voltage(int sensor_index) {
  select_sensor(sensor_index); 
  String dataString;
  Wire.requestFrom(8, 7); // request 7 bytes of data from peripheral #8 // blocking function. 
  while (Wire.available()) {
    char c = Wire.read();
    dataString = dataString + c;
  }
  float result = dataString.toFloat();
  if(debug) Serial.println(result);
  return result; 
}

void loop() {
  // REQUEST SENSOR 1 READING 
  request_voltage(1); 
  delay(1000);

  // REQUEST SENSOR 2 READING 
  request_voltage(2); 
  delay(1000);
}
