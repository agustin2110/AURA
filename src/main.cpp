#include <WiFi.h>
#include <time.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

// Datos WiFi
const char* ssid = "Wokwi-GUEST";
const char* password = "";


// Configuración MAX7219
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4   // cantidad de módulos 8x8
#define CS_PIN 5

MD_Parola matriz = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

// Zona horaria Argentina
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -3 * 3600;
const int daylightOffset_sec = 0;

char mensaje[60];

void setup() {
  Serial.begin(115200);

  matriz.begin();
  matriz.setIntensity(5);   // brillo 0 a 15
  matriz.displayClear();
  matriz.displayText("Conectando WiFi...", PA_CENTER, 80, 1000, PA_SCROLL_LEFT, PA_SCROLL_LEFT);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi conectado");

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  matriz.displayClear();
}

void loop() {
  struct tm timeinfo;

  if (!getLocalTime(&timeinfo)) {
    strcpy(mensaje, "Error hora");
  } else {
    // Formato: 24/04/2026 16:45
    sprintf(
      mensaje,
      "%02d/%02d/%04d  %02d:%02d",
      timeinfo.tm_mday,
      timeinfo.tm_mon + 1,
      timeinfo.tm_year + 1900,
      timeinfo.tm_hour,
      timeinfo.tm_min
    );
  }

  matriz.displayText(mensaje, PA_CENTER, 80, 1000, PA_SCROLL_LEFT, PA_SCROLL_LEFT);

  while (!matriz.displayAnimate()) {
    delay(10);
  }

  delay(500);
}