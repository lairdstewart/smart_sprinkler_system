// Feather9x_RX
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messaging client (receiver)
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example Feather9x_TX

#include <SPI.h>
#include <RH_RF95.h>

// feather m0 RFM9x pins
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3
#define LED 13 // built in red LED

// feather m0 RFM9x frequency
#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);
uint8_t sensor_address = 2;

void setup()
{
  pinMode(A0, INPUT); // sensor reading 
  pinMode(LED, OUTPUT);
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  led_blink_delay(LED, 3, 1);

  Serial.begin(115200);
  while (!Serial) {
    delay(1);
  }
  delay(100);

  Serial.println("Feather LoRa RX Test!");

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

void led_blink_delay(int led_pin, int time_s, int end_state) {
  for(int i=0; i < time_s; i++) {
    digitalWrite(led_pin, HIGH);
    delay(500);
    digitalWrite(led_pin, LOW);
    delay(500); 
  }
  digitalWrite(led_pin, end_state); 
}

void loop()
{

  if (rf95.available())
  {
    // Should be a message for us now
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    if (rf95.recv(buf, &len))
    {
      // Recieve message 
      RH_RF95::printBuffer("Received: ", buf, len);
      // char * msg = (char*) buf;
      Serial.print("Got: ");
      // Serial.println(msg);
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);

      char message_address = buf[0];
      Serial.print("buf[0]: "); 
      Serial.println(buf[0]); 
      Serial.print("address recieved: "); 
      Serial.println(message_address); 
      if (buf[0] == sensor_address) {
        // read sensor
        int sensorval = analogRead(A0);
        Serial.print("sensor val: ");
        Serial.println(sensorval); 
        char char_data[5];
        itoa(sensorval, char_data, 10); // puts integer charachters into a char array (decimal) 
        
        uint8_t* data = (uint8_t*) char_data;  // turn char array into byte array 

        // Send a reply
        // uint8_t data[] = "And hello back to you";
        rf95.send(data, sizeof(data));
        rf95.waitPacketSent();
        Serial.println("Sent a reply");
        led_blink_delay(LED, 1, 1); 
      }
      

    }
    else
    {
      Serial.println("Receive failed");
    }
  }
}



