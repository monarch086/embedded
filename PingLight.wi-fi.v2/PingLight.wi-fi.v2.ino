/*
 * HTTPS Secured Client POST Request
 * Copyright (c) 2019, circuits4you.com
 * All rights reserved.
 * https://circuits4you.com 
 * Connects to WiFi HotSpot. */

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h> 
//#include <ESP8266WebServer.h>
//#include <ESP8266HTTPClient.h>

/* Set these to your desired credentials. */
// const char *ssid = "Galaxy A72ED35";
// const char *password = "ngxk5523";

const char* ssid = "Lamborghini";
const char* password = "ghj82RTp";

//Link to read data from https://jsonplaceholder.typicode.com/comments?postId=7
//Web/Server address to read/write from 
const char *host = "https://4f7ahkyqt26wng75ossxz4ndae0rvpsb.lambda-url.eu-central-1.on.aws";
const int httpsPort = 443;  //HTTPS= 443 and HTTP = 80

//SHA1 finger print of certificate use web browser to view and copy
const char fingerprint[] PROGMEM = "CD 50 05 4F F8 6C E8 F7 6E DE 21 8E 80 93 2C 96 80 87 0A E3";
//=======================================================================
//                    Power on setup
//=======================================================================

void setup() {
  delay(1000);
  Serial.begin(115200);
  WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
  delay(1000);
  WiFi.mode(WIFI_STA);        //Only Station No AP, This line hides the viewing of ESP as wifi hotspot
  
  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");

  Serial.print("Connecting");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
}

//=======================================================================
//                    Main Program Loop
//=======================================================================
void loop() {
  WiFiClientSecure httpsClient;    //Declare object of class WiFiClient

  Serial.println(host);

  Serial.printf("Using fingerprint '%s'\n", fingerprint);
  httpsClient.setFingerprint(fingerprint);
  httpsClient.setTimeout(15000); // 15 Seconds
  delay(1000);
  
  Serial.print("HTTPS Connecting");
  int r=0; //retry counter
  while((!httpsClient.connect(host, httpsPort)) && (r < 30)){
      delay(1000);
      Serial.print(".");
      r++;
  }
  if(r==30) {
    Serial.println("Connection failed");
  }
  else {
    Serial.println("Connected to web");
  }
  
  String getData, Link;
  
  //POST Data
  Link = "/?Id=V61-45";

  Serial.print("requesting URL: ");
  Serial.println(host);
  /*
   POST /post HTTP/1.1
   Host: postman-echo.com
   Content-Type: application/x-www-form-urlencoded
   Content-Length: 13
  
   say=Hi&to=Mom
    
   */

  httpsClient.print(String("POST ") + Link + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Content-Type: application/x-www-form-urlencoded"+ "\r\n" +
               //"Content-Length: 13" + "\r\n\r\n" +
               //"say=Hi&to=Mom" + "\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("request sent");
                  
  while (httpsClient.connected()) {
    String line = httpsClient.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }

  Serial.println("reply was:");
  Serial.println("==========");
  String line;
  while(httpsClient.available()){        
    line = httpsClient.readStringUntil('\n');  //Read Line by Line
    Serial.println(line); //Print response
  }
  Serial.println("==========");
  Serial.println("closing connection");
    
  delay(2000);  //POST Data at every 2 seconds
}
//=======================================================================