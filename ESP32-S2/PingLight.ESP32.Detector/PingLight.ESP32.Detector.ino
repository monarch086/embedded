#include <WiFiClientSecure.h>

const char* ssid = "TP-Link_67A7";
const char* password = "vxMEs58^";

// const char* ssid = "Galaxy A72ED35";
// const char* password = "ngxk5523";


const char* host = "https://4f7ahkyqt26wng75ossxz4ndae0rvpsb.lambda-url.eu-central-1.on.aws";
const int httpsPort = 443;

const String deviceId = "test";
//const String deviceId = "S4D-12";
const String query = "/?Id=" + deviceId;
const String url = host + query;

const String body = "{\"Id\": " + deviceId + "}";

void setup() {
  Serial.begin(115200);

  Serial.println();
  Serial.println("=== PingLightDetector ESP8266 ===");
  Serial.println();
  Serial.println("MAC address: ");
  Serial.println(WiFi.macAddress());
  Serial.printf("Connecting to %s\n", ssid);

  WiFi.begin(ssid, password);
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

void loop() {
  unsigned short count = 0;

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
    count++;

    if (count >= 60) {
      wifiRestart();
      count = 0;
    }
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  WiFiClientSecure httpsClient;

  Serial.println(url);

  httpsClient.setInsecure();

  httpsClient.setTimeout(15000);  // 15 Seconds
  delay(1000);

  Serial.print("HTTPS Connecting");
  int r = 0;  //retry counter
  while ((!httpsClient.connect(host, httpsPort)) && (r < 60)) {
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

  

  Serial.print("[HTTP] end...\n");
  httpsClient.stop();

  Serial.print("Sleep for 30 sec...\n");
  delay(30000);
  // ESP.deepSleep(30e6);
}