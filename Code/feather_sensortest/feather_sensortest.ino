// the setup function runs once when you press reset or power the board
void setup() {
//  pinMode(13, OUTPUT); // built in red LED
  pinMode(A0, INPUT);
  Serial.println("SENSOR TEST:\n"); 
}

// the loop function runs over and over again forever
void loop() {
  int sensorval = analogRead(A0); 
  Serial.println(sensorval);
  delay(1000); 
}
