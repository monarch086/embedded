#include <Adafruit_BMP085.h>
#include <Adafruit_SHT31.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Arduino_JSON.h>     //https://github.com/arduino-libraries/Arduino_JSON

#define seaLevelPressure_hPa 1013.25

Adafruit_BMP085 bmp;
Adafruit_SHT31 sht31 = Adafruit_SHT31();

const char* ssid = "TP-Link_67A7";
const char* password = "vxMEs58^";

const char* host = "https://2zv6u4o42uhgvhkuxjzi3yk3ha0toubx.lambda-url.eu-central-1.on.aws";
const int httpsPort = 443;

const String deviceId = "S4D-12";
const String url = host;

void setup() {
  Serial.begin(115200);
  delay(10);

  if (!bmp.begin() || !sht31.begin(0x44)) {
    Serial.println("Sensors not found. CHECK CIRCUIT!");
    while (1) {}
  }

  WiFi.begin(ssid, password);
}

void loop() {
  float t = sht31.readTemperature();
  float h = sht31.readHumidity();

  Serial.print("[BMP085] Temperature = ");
  Serial.print(bmp.readTemperature());
  Serial.println(" *C");

  Serial.print("[BMP085] Pressure = ");
  Serial.print(bmp.readPressure());
  Serial.println(" Pa");

  Serial.print("[BMP085] Pressure at sealevel (calculated) = ");
  Serial.print(bmp.readSealevelPressure());
  Serial.println(" Pa");

  Serial.print("[BMP085] Altitude = ");
  Serial.print(bmp.readAltitude());
  Serial.println(" meters");

  Serial.print("[BMP085] Real altitude = ");
  Serial.print(bmp.readAltitude(seaLevelPressure_hPa * 100));
  Serial.println(" meters");

  if (!isnan(t)) {
    Serial.print("[SHT31] Temp *C = ");
    Serial.println(t);
  } else {
    t = 0.0;
    Serial.println("[SHT31] Failed to read temperature");
  }

  if (!isnan(h)) {
    Serial.print("[SHT31] Hum. % = ");
    Serial.println(h);
  } else {
    h = 0.0;
    Serial.println("[SHT31] Failed to read humidity");
  }

  Serial.println();

  sendData();
  
  delay(30000);
}

void sendData() {
    unsigned short count = 0;

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    count++;

    if (count >= 30) {
      wifiRestart();
      count = 0;
    }
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  HTTPClient http;
  WiFiClientSecure httpsClient;

  Serial.println(url);

  httpsClient.setInsecure();

  httpsClient.setTimeout(15000);  // 15 Seconds
  delay(1000);

  Serial.print("HTTPS Connecting");
  int r = 0;  //retry counter
  while ((!httpsClient.connect(host, httpsPort)) && (r < 30)) {
    delay(1000);
    Serial.print(".");
    r++;
  }
  Serial.println();
  if (r == 30) {
    Serial.println("HTTPS connection failed");
    return;
  } else {
    Serial.println("HTTPS connection is successful");
  }

  Serial.print("[HTTP] begin...\n");
  http.begin(httpsClient, url);
  http.addHeader("Content-Type", "application/json");

  String body = createBody(deviceId, bmp);
  Serial.printf("Body: %s\n", body.c_str());

  int httpCode = http.POST(body);
  if (httpCode > 0) {
    http.writeToStream(&Serial);

    Serial.printf("[HTTP] ... code: %d\n", httpCode);

    if (httpCode >= 200 and httpCode <= 299) {
      String payload = http.getString();
      Serial.printf("Payload: %s\n", payload);
    }
  } else {
    Serial.printf("[HTTP] ... failed, error: %s\n", http.errorToString(httpCode).c_str());
    String payload = http.getString();
    Serial.printf("Payload: %s\n", payload);
  }

  Serial.print("[HTTP] end...\n");
  http.end();

  Serial.print("Sleep for 30 sec...\n");
}

void wifiRestart() {
  Serial.println("Restarting Wi-Fi: turning WiFi off...");
  WiFi.mode(WIFI_OFF);
  Serial.println("Sleeping for 10 seconds...");
  delay(10000);
  Serial.println("Trying to connect to WiFi...");
  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid, password);
}

// String createBody(String deviceId, Adafruit_BMP085 bmp) {
//   String body = "{\"DeviceId\": \"" + deviceId + "\", ";
//   body += "\"BMP085\": {";
//   body += "\"Temperature\": " + String(bmp.readTemperature()) + ", ";
//   body += "\"Pressure\": " + String(bmp.readPressure()) + "}";
//   body += "}";

//   return body;
// }

String createBody(String deviceId, Adafruit_BMP085 bmp) {
  JSONVar requestBody;

  requestBody["DeviceId"] = deviceId;
  requestBody["BMP085"]["Temperature"] = bmp.readTemperature();
  requestBody["BMP085"]["Pressure"] = bmp.readPressure();

  return JSON.stringify(requestBody);
}



