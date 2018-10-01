#include <ESP8266WiFi.h>

#include "connection.h"

const uint8_t ESP8266_LED = 5;


void setup() {
  pinMode(ESP8266_LED, OUTPUT);

  Serial.begin(115200);
  delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

bool on = false;

void loop() {
  delay(5000);
  on = !on;

  Serial.print("Connecting to ");
  Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("Connection failed");
    return;
  }
  
  // We now create a URI for the request
  String url = "/api/XF8h6vC53cJdur1IGWDcoTnpRhvA6ZFRbhwL9o6l/lights/2/state";

  String command; 
  
  if (on) {
    command = "{\"on\": true, \"transitiontime\": 20, \"bri\": 254, \"hue\": 10000, \"sat\": 254}";
  
    digitalWrite(ESP8266_LED, HIGH);
  } else {
    command = "{\"on\": false, \"transitiontime\": 50}";
  
    digitalWrite(ESP8266_LED, LOW);
  }
  
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // This will send the request to the server
  client.print(String("PUT ") + url + " HTTP/1.1\r\n" +
               "Connection: Keep-Alive\r\n" +
               "Host: " + host + "\r\n"+
               "Content-Length: " + command.length() +  "\r\n" +
               "Content-Type: text/plain;charset=UTF-8\r\n\r\n" +
               command);

  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println();
  Serial.println("closing connection");
}
