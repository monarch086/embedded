/*
    HTTP over TLS (HTTPS) example sketch

    This example demonstrates how to use
    WiFiClientSecure class to access HTTPS API.
    We fetch and display the status of
    esp8266/Arduino project continuous integration
    build.

    Created by Ivan Grokhotkov, 2015.
    This example is in public domain.
*/

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include "certs.h"

#ifndef STASSID
#define STASSID "Lamborghini"
#define STAPSK "ghj82RTp"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

const char* host = "https://4f7ahkyqt26wng75ossxz4ndae0rvpsb.lambda-url.eu-central-1.on.aws";
const int httpsPort = 443;  //HTTPS= 443 and HTTP = 80
const char fingerprint[] PROGMEM = "CD 50 05 4F F8 6C E8 F7 6E DE 21 8E 80 93 2C 96 80 87 0A E3";


X509List cert(cert_DigiCert_Global_Root_CA);

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
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

  // Set time via NTP, as required for x.509 validation
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));

  // Use WiFiClientSecure class to create TLS connection
  WiFiClientSecure client;
  Serial.print("Connecting to ");
  Serial.println(host);

  //Serial.printf("Using certificate: %s\n", cert_DigiCert_Global_Root_CA);
  //client.setTrustAnchors(&cert);
  client.setFingerprint(fingerprint);

  if (!client.connect(host, httpsPort)) {
    Serial.println("Connection failed");
    return;
  }

  //String url = "/repos/esp8266/Arduino/commits/master/status";
  String url = "/?Id=V61-45";
  Serial.print("Requesting URL: ");
  Serial.println(url);

  client.print(String("POST ") + url + " HTTP/1.1\r\n" + 
                        "Host: " + host + 
                        "\r\n" + "User-Agent: PingLightDetectorESP8266\r\n" +
                        "Content-Type: text/plain"+ "\r\n" +
                        "Connection: close\r\n\r\n");

  Serial.println("Request sent");
  while (client.available()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("Headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  // if (line.startsWith("{\"state\":\"success\"")) {
  //   Serial.println("esp8266/Arduino CI successful!");
  // } else {
  //   Serial.println("esp8266/Arduino CI has failed");
  // }
  Serial.println("Reply was:");
  Serial.println("==========");
  Serial.println(line);
  Serial.println("==========");
  Serial.println("Closing connection");
}

void loop() {}
