//following Peripheral Sender from: https://docs.arduino.cc/learn/communication/wire
#include <Wire.h>
#include <avr/dtostrf.h>

float voltage_readings[3] = {-1, 1.1, 2.2}; // for the demo we will only have 2 sensors 
byte requested_sensor = 0; 
bool debug = 1;

void setup() {
  // put your setup code here, to run once:
  Wire.begin(8); // peripheral #8
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);  
}

void loop() {
  // delay(100); // delay() blocks, i think it would block an interrupt from happening.  
}

void receiveEvent(int howMany) {
  /*
  howMany: number of bytes available to be read from the I2C bus. Same as Wire.available()
  - The arduino controller will only ever send one byte representing which sensor reading it wants.
  - Update requested_sensor to this value. 
  */ 

  if (howMany != 1) {
    if(debug) Serial.print("error, howMany == "); // debug
    if(debug) Serial.println(howMany); 
    return;
  }
  requested_sensor = Wire.read();
  Serial.print("recieve event. Requested sensor: ");
  Serial.println(requested_sensor); 
}
  
void requestEvent() {
  /*
  - The arduino controller will only request 7 bytes 
  - Send the most recient voltage reading from requested_sensor
  */
  char buffer[7];
  float voltage = voltage_readings[requested_sensor]; 
  dtostrf(voltage, -7, 5, buffer); // -7 left aligns the bytes. 7 bytes long, 5 decimal point precision 
  Wire.write(buffer);
  if(debug) Serial.print("request event. value returned: ");
  if(debug) Serial.println(voltage);
}
