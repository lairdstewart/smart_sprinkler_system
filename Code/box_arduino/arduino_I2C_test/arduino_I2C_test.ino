//following Controller Reader from: https://docs.arduino.cc/learn/communication/wire
#include <Wire.h>

void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  String dataString = "";
  float voltage = 0.0F;

  Wire.requestFrom(8, 7);
  while (Wire.available())
  {
    char c = Wire.read();
    dataString = dataString + c;
    Serial.print(c);
  }
  
  voltage = dataString.toFloat();
  Serial.println(voltage);

  delay(1000);
}
