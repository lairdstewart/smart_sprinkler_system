//following Peripheral Sender from: https://docs.arduino.cc/learn/communication/wire
#include <Wire.h>
#include <avr/dtostrf.h>
#include <SPI.h>
#include <RH_RF95.h>


// =========================== DEFINES ===========================
// for feather m0  
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3
#define RF95_FREQ 915.0
#define led 13

// =========================== GLOBALS ===========================
bool debug = 1;
int requested_sensor = 0; // sensor index requested

// lora 
int16_t packetnum = 0;  // packet counter, we increment per xmission
RH_RF95 rf95(RFM95_CS, RFM95_INT); // Singleton instance of the radio driver


// =========================== SETUP ===========================
void setup() {
  pinMode(led, OUTPUT);
  led_blink_delay(led, 3); 
  digitalWrite(led, HIGH); 

  // lora 
  pinMode(RFM95_RST, OUTPUT);
  pinMode(led, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  lora_setup(); 

  // i2c
  Wire.begin(8); // peripheral #8
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent); // register event

  Serial.begin(9600);
  while (!Serial) {
    delay(1);
  }
  delay(100);

  Serial.println("seutp complete"); 
}

void lora_setup() {
    // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    Serial.println("Uncomment '#define SERIAL_DEBUG' in RH_RF95.cpp for detailed debug info");
    while (1);
  }
  Serial.println("LoRa radio init OK!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
}


// =========================== HELPERS ===========================
void led_blink_delay(int led_pin, int time_s) {
  for(int i=0; i < time_s; i++) {
    digitalWrite(led_pin, HIGH);
    delay(500);
    digitalWrite(led_pin, LOW);
    delay(500); 
  }
}

void send_request(byte sensor_index) {
  digitalWrite(led, LOW); 
  char radiopacket[1] = {sensor_index}; 
  Serial.println("Sending...");
  delay(10);
  rf95.send((uint8_t *)radiopacket, 20);
  Serial.println("Waiting for packet to complete..."); 
  delay(10);
  rf95.waitPacketSent(); // waiting for entirety of packet to be sent 
  digitalWrite(led, HIGH); 
}


// =========================== MAIN LOOP ===========================
void loop() {
  // delay(100); // delay() blocks, i think it would block an interrupt from happening.  
}


// =========================== INTERRUPTS ===========================
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
  requested_sensor = Wire.read(); // TODO might break 
  Serial.print("recieve event. Requested sensor: ");
  Serial.println(requested_sensor); 
}
  
void requestEvent() {
  /*
  - The arduino controller will only request 7 bytes 
  - Send the most recient voltage reading from requested_sensor
  */
  
  // LORA REQUEST TO SENSOR
  send_request(requested_sensor); 

  // LORA RECIEPT FROM SENSOR``
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  Serial.println("Waiting for reply...");
  if (rf95.waitAvailableTimeout(1000))
  { 
    // Should be a reply message for us now   
    if (rf95.recv(buf, &len))
   {
     
      Serial.print("(SUCCESS) Got reply: ");
      Serial.println((char*)buf);
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);    
    }
    else
    {
      Serial.println("Receive failed");
    }
  }
  else
  {
    Serial.println("No reply, is there a listener around?\n");
  }

  // create char[] message
  char* message = (char*) buf; 

  // char buffer[7];
  // dtostrf(voltage, -7, 5, buffer); // -7 left aligns the bytes. 7 bytes long, 5 decimal point precision 
  
  // send i2c message 
  Wire.write(message);
  if(debug) Serial.print("request event. value returned: ");
  if(debug) Serial.println(message);
}
