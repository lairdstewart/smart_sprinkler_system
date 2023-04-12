#include <Wire.h>
#include <SPI.h>
#include <WiFiNINA.h>

// =============== GLOBAL VARIABLES =============== 
bool debug = 1; 
char ssid[] = "Mehul's Phone";    // your network SSID (name)
char pass[] = "Mehul123";         // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                 // your network key index number (needed only for WEP)
bool relay1 = 0;
bool relay2 = 0;
bool autoMode = 0;
float sensor1_moisture = 0; 
float sensor2_moisture = 0; 
float min_voltage = 0.0F;
float max_voltage = 1000.0F;
int threshold = 50;
int threshold_band = 5;
int status = WL_IDLE_STATUS;
WiFiServer server(80);

// =============== SETUP =============== 
void setup() {
  Wire.begin(); // join i2c bus
  Serial.begin(9600); // serial output to console
  pinMode(9, OUTPUT);      // set the LED pin mode
  setup_wifi(); 
}

void setup_wifi() {
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  Serial.println("firmware version: " + fv); 
  Serial.print("latest firmware version: ");
  Serial.println(WIFI_FIRMWARE_LATEST_VERSION);
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);                   // print the network name (SSID);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10000);
  }
  server.begin();                           // start the web server on port 80
  printWifiStatus();                        // you're connected now, so print out the status
}

// =============== HELPER METHODS =============== 
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
  if(debug) Serial.println("requested voltage over i2c"); 
  if(debug) Serial.println(dataString);
  if(debug) Serial.println(result);
  return result; 
}

float request_moisture(int sensor_index) {
  float voltage = request_voltage(sensor_index);
  float moisture = (voltage - min_voltage) / (max_voltage - min_voltage) * 100.0F;
  return moisture;
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}

// =============== MAIN =============== 

void loop() {
  WiFiClient client = server.available();   // listen for incoming clients

  // automatic mode code here. TODO 

  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("Click <a href=\"/A\">here</a> Toggle Sprinkler #1<br>");
            client.print("Click <a href=\"/B\">here</a> Toggle Sprinkler #2<br>");
            client.print("Click <a href=\"/E\">here</a> Toggle Automode<br>");

            // poll sensors for their voltages
            sensor1_moisture = request_moisture(1); 
            sensor2_moisture = request_moisture(2); 
            // sensor2_moisture = 2.22F; 
            client.print("Sensor #1 moisture reading:"); 
            client.print(sensor1_moisture); 
            client.print("%<br>");
            // client.print("Sensor #2 moisture reading:"); 
            // client.print(sensor2_moisture); 
            // client.print("%<br>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /A")) {
          autoMode = 0;
          relay1 = !relay1;
          Serial.println("got 00"); 
        }
        if (currentLine.endsWith("GET /B")) {
          autoMode = 0;
          relay2 = !relay2;
          Serial.println("got 01"); 
        }
        if (currentLine.endsWith("GET /E")) {
          autoMode = !autoMode;
          Serial.println("got A"); 
        }

      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  } 
  if (autoMode) {
      sensor1_moisture = request_moisture(1); 
      sensor2_moisture = request_moisture(2); 
      if (sensor1_moisture < (threshold - threshold_band))
        relay1 = 1;
      if (sensor1_moisture > (threshold + threshold_band))
        relay1 = 0;
      if (sensor2_moisture < (threshold - threshold_band))
        relay2 = 1;
      if (sensor2_moisture > (threshold + threshold_band))
        relay2 = 0;
  }
  // digital out (relay1_pin, relay1)
  // digital out (relay2_pin, relay2)
  
}
