//#include <ESP8266HTTPClient.h>

//#include <ArduinoHttpClient.h>

/*
  Rui Santos
  Complete project details at Complete project details at https://RandomNerdTutorials.com/esp32-http-get-post-arduino/

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

// #include <WiFi.h>
// #include <HttpClient.h>
//#include <Arduino_JSON.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

const char* ssid = "Lamborghini";
const char* password = "ghj82RTp";

//Your Domain name with URL path or IP address with path
const char* serverName = "https://4f7ahkyqt26wng75ossxz4ndae0rvpsb.lambda-url.eu-central-1.on.aws/?Id=12";
const unsigned char* fingerprint = reinterpret_cast<const unsigned char *>( "51 87 10 92 52 38 41 A9 SD 23 V3 23 72 58 SE EF 06 8D 35 4C" );

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
// unsigned long timerDelay = 5000;
unsigned long timerDelay = 10000; // 10 sec

String sensorReadings;
float sensorReadingsArr[3];

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
 
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
}

void loop() {
  //Send an HTTP POST request every 10 minutes
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){     
      sensorReadings = httpPOSTRequest(serverName);
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}

String httpPOSTRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;
    
  // Your Domain name with URL path or IP address with path
  http.begin(client, serverName);
  
  // If you need Node-RED/server authentication, insert user and password below
  //http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");
  
  // Send HTTP POST request
  // const String headerName = "host";
  // const String headerValue = "developer.mozilla.org:443";
  // http.addHeader(headerName, headerValue);
  // http.addHeader("Content-Type", "text/plain");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  String httpRequestData = "Id=12";
  int httpResponseCode = http.POST(httpRequestData);
  
  String payload = "{}"; 
  
  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
    Serial.println(payload);
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}