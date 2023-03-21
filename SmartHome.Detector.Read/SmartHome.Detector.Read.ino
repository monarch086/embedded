#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <Adafruit_SHT31.h>
#define seaLevelPressure_hPa 1013.25

Adafruit_BMP085 bmp;
Adafruit_SHT31 sht31 = Adafruit_SHT31();

void setup() {
  Serial.begin(115200);
  delay(10);

  if (!bmp.begin() || !sht31.begin(0x44)) {
    Serial.println("Sensors not found. CHECK CIRCUIT!");
    while (1) {}
  }
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
  delay(5000);
}