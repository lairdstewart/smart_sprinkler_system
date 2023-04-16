#define relay1 4
#define relay2 5

void setup() {
  // put your setup code here, to run once:
  pinMode(relay1, OUTPUT); 
  pinMode(relay2, OUTPUT); 
}

void loop() {
  // NOTE: use the two pins on the SONGLE side.  these are noramlly open. 
  // NOTE: LOW powers the relay (to connect it), HIGH disconnects it -- it is opposite of what you think
  // NOTE: when the LED is on on the relay board, that indicates that the relay is closed. 
  digitalWrite(relay1, HIGH); 
  delay(1000);
  digitalWrite(relay1, LOW);
  delay(1000);
  digitalWrite(relay2, HIGH);
  delay(1000);
  digitalWrite(relay2, LOW);
  delay(1000);

}
