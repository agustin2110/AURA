#pragma once

#include <Arduino.h>
#include <MD_MAX72xx.h>

// ==========================
// WIFI
// ==========================
static const char* WIFI_SSID = "Wokwi-GUEST";
static const char* WIFI_PASSWORD = "";

// ==========================
// PINES HARDWARE
// ==========================
#define HARDWARE_TYPE MD_MAX72XX::PAROLA_HW
#define MAX_DEVICES 4

#define MATRIX_DIN_PIN 23
#define MATRIX_CLK_PIN 18
#define MATRIX_CS_PIN 5

#define BMP_SDA_PIN 21
#define BMP_SCL_PIN 22

#define IR_PIN 4
#define BUZZER_PIN 33
#define BOTON_PIN 25
#define RELE_PIN 0

// Si tu modulo rele es activo en LOW, cambia estos valores:
#define RELE_ON HIGH
#define RELE_OFF LOW

// Boton con pull-down externo:
// sin presionar = LOW, presionado = HIGH
static const unsigned long TIEMPO_PULSACION_LARGA = 1200;

// Hora Argentina
static const char* NTP_SERVER = "pool.ntp.org";
static const long GMT_OFFSET_SEC = -3 * 3600;
static const int DAYLIGHT_OFFSET_SEC = 0;
