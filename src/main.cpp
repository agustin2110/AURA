#include <WiFi.h>
#include <time.h>
#include <Wire.h>
#include <Adafruit_BMP085_U.h>

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

// WiFi
const char* ssid = "TU_WIFI";
const char* password = "TU_CLAVE";

// MAX7219
#define HARDWARE_TYPE MD_MAX72XX::PAROLA_HW
#define MAX_DEVICES 4
#define CS_PIN 5

MD_Parola matriz = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

// BMP180
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

// Hora Argentina
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -3 * 3600;
const int daylightOffset_sec = 0;

char mensaje[100];

void setup() {
  Serial.begin(115200);

  Wire.begin(21, 22);

  matriz.begin();
  matriz.setIntensity(5);
  matriz.displayClear();

  matriz.displayText("Iniciando...", PA_CENTER, 80, 1000, PA_SCROLL_LEFT, PA_SCROLL_LEFT);

  if (!bmp.begin()) {
    matriz.displayClear();
    matriz.print("Error BMP180");
    while (1);
  }

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  matriz.displayClear();
}

void loop() {
  struct tm timeinfo;

  float temperatura;
  bmp.getTemperature(&temperatura);
  float presion;
  bmp.getPressure(&presion);

  if (!getLocalTime(&timeinfo)) {
    sprintf(
      mensaje,
      "Temp: %.1f C  Presion: %.0f hPa",
      temperatura,
      presion
    );
  } else {
    sprintf(
      mensaje,
      "%02d/%02d/%04d  %02d:%02d  Temp: %.1f C  Presion: %.0f hPa",
      timeinfo.tm_mday,
      timeinfo.tm_mon + 1,
      timeinfo.tm_year + 1900,
      timeinfo.tm_hour,
      timeinfo.tm_min,
      temperatura,
      presion
    );
  }

  matriz.displayText(mensaje, PA_CENTER, 80, 1000, PA_SCROLL_LEFT, PA_SCROLL_LEFT);

  while (!matriz.displayAnimate()) {
    delay(10);
  }

  delay(500);
}