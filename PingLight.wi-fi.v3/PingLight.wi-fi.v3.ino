#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "Lamborghini";
const char* password = "ghj82RTp";

// const char* ssid = "Galaxy A72ED35";
// const char* password = "ngxk5523";

const char* host = "https://4f7ahkyqt26wng75ossxz4ndae0rvpsb.lambda-url.eu-central-1.on.aws";
const int httpsPort = 443;

const String deviceId = "V61-45";
const String query = "/?Id=" + deviceId;
const String url = host + query;

// const char fingerprint[] PROGMEM = "CD 50 05 4F F8 6C E8 F7 6E DE 21 8E 80 93 2C 96 80 87 0A E3";

void setup() {
  Serial.begin(115200);

  Serial.println();
  Serial.println("=== PingLightDetector v3 ===");
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

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

  // Serial.printf("Using fingerprint '%s'\n", fingerprint);
  // httpsClient.setFingerprint(fingerprint);
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
    Serial.println("Connection failed");
    return;
  } else {
    Serial.println("Connected to web");
  }

  Serial.print("[HTTPS] begin...\n");
  http.begin(httpsClient, url);
  http.addHeader("Content-Type", "application/json");

  int httpCode = http.POST("{\"hash\": \"34jh34uh3v\"}");
  if (httpCode > 0) {
    http.writeToStream(&Serial);

    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTPS] ... code: %d\n", httpCode);

    if (httpCode >= 200 and httpCode <= 299) {
      String payload = http.getString();
      Serial.printf("Payload: %s\n", payload);
    }
  } else {
    Serial.printf("[HTTPS] ... failed, error: %s\n", http.errorToString(httpCode).c_str());
    String payload = http.getString();
    Serial.printf("Payload: %s\n", payload);
  }

  delay(30000);  //30 sec

  Serial.print("[HTTPS] end...\n");
  http.end();
}