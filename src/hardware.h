#pragma once

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <Adafruit_BMP085_U.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPDash.h>

extern MD_Parola matriz;
extern Adafruit_BMP085_Unified bmp;
extern IRsend irsend;
extern AsyncWebServer server;
extern ESPDash dashboard;
