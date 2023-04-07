//following Peripheral Sender from: https://docs.arduino.cc/learn/communication/wire
#include <Wire.h>
#include <avr/dtostrf.h>

float voltage = 0.0F;
char voltBuff[7];

void setup() {
  // put your setup code here, to run once:
  Wire.begin(8);
  Wire.onRequest(requestEvent);
}

void loop() {
  // put your main code here, to run repeatedly:
  //simulate geting data from sensors
  voltage = voltage + 0.0001F;
  if (voltage >= 100)
    voltage = 0.0F;
  dtostrf(voltage,-7,5, voltBuff); //7 byte maximum
  delay(100);
}

void requestEvent() {
  Wire.write(voltBuff);
}
