#include <WiFi.h>
#include <time.h>
#include <Wire.h>
#include <SPI.h>

#include "config.h"
#include "state.h"
#include "hardware.h"
#include "ui_cards.h"
#include "ir_lampara.h"
#include "alarmas.h"
#include "recordatorios.h"
#include "boton.h"
#include "display_control.h"

// ==========================
// OBJETOS HARDWARE
// ==========================
MD_Parola matriz = MD_Parola(HARDWARE_TYPE, MATRIX_CS_PIN, MAX_DEVICES);
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);
IRsend irsend(IR_PIN);

AsyncWebServer server(80);
ESPDash dashboard(server);

// ==========================
// CARDS ESP-DASH
// ==========================

// Lampara RGB
dash::SeparatorCard<dash::string> seccionLamparaRGB(dashboard, "Lampara RGB", "Control por infrarrojo");
dash::ToggleButtonCard btnOn(dashboard, "ENCENDIDO");
dash::ToggleButtonCard btnOff(dashboard, "APAGADO");
dash::ToggleButtonCard btnBmas(dashboard, "Brillo +");
dash::ToggleButtonCard btnBmenos(dashboard, "Brillo -");
dash::ToggleButtonCard btnR(dashboard, "Rojo");
dash::ToggleButtonCard btnG(dashboard, "Verde");
dash::ToggleButtonCard btnB(dashboard, "Azul");
dash::ToggleButtonCard btnW(dashboard, "Blanco");
dash::ToggleButtonCard btnFlash(dashboard, "FLASH");
dash::ToggleButtonCard btnStrobe(dashboard, "STROBE");
dash::ToggleButtonCard btnFade(dashboard, "FADE");
dash::ToggleButtonCard btnSmooth(dashboard, "SMOOTH");

// Alarmas
dash::SeparatorCard<dash::string> seccionAlarmas(dashboard, "Alarmas", "Configuracion de alarmas");

// Alarma 1
dash::SeparatorCard<dash::string> seccionAlarma1(dashboard, "Alarma 1", "Hora, minuto, duracion y dias");
dash::SliderCard<int> alarma1Hora(dashboard, "Alarma 1 - Hora", 0, 23, 1, "hs");
dash::SliderCard<int> alarma1Minuto(dashboard, "Alarma 1 - Minuto", 0, 59, 1, "min");
dash::SliderCard<int> alarma1Duracion(dashboard, "Alarma 1 - Duracion", 1, 300, 1, "seg");
dash::ToggleButtonCard alarma1Activa(dashboard, "Activar alarma 1");
dash::ToggleButtonCard alarma1Dom(dashboard, "A1 Domingo");
dash::ToggleButtonCard alarma1Lun(dashboard, "A1 Lunes");
dash::ToggleButtonCard alarma1Mar(dashboard, "A1 Martes");
dash::ToggleButtonCard alarma1Mie(dashboard, "A1 Miercoles");
dash::ToggleButtonCard alarma1Jue(dashboard, "A1 Jueves");
dash::ToggleButtonCard alarma1Vie(dashboard, "A1 Viernes");
dash::ToggleButtonCard alarma1Sab(dashboard, "A1 Sabado");

// Alarma 2
dash::SeparatorCard<dash::string> seccionAlarma2(dashboard, "Alarma 2", "Hora, minuto, duracion y dias");
dash::SliderCard<int> alarma2Hora(dashboard, "Alarma 2 - Hora", 0, 23, 1, "hs");
dash::SliderCard<int> alarma2Minuto(dashboard, "Alarma 2 - Minuto", 0, 59, 1, "min");
dash::SliderCard<int> alarma2Duracion(dashboard, "Alarma 2 - Duracion", 1, 300, 1, "seg");
dash::ToggleButtonCard alarma2Activa(dashboard, "Activar alarma 2");
dash::ToggleButtonCard alarma2Dom(dashboard, "A2 Domingo");
dash::ToggleButtonCard alarma2Lun(dashboard, "A2 Lunes");
dash::ToggleButtonCard alarma2Mar(dashboard, "A2 Martes");
dash::ToggleButtonCard alarma2Mie(dashboard, "A2 Miercoles");
dash::ToggleButtonCard alarma2Jue(dashboard, "A2 Jueves");
dash::ToggleButtonCard alarma2Vie(dashboard, "A2 Viernes");
dash::ToggleButtonCard alarma2Sab(dashboard, "A2 Sabado");

// Alarma 3
dash::SeparatorCard<dash::string> seccionAlarma3(dashboard, "Alarma 3", "Hora, minuto, duracion y dias");
dash::SliderCard<int> alarma3Hora(dashboard, "Alarma 3 - Hora", 0, 23, 1, "hs");
dash::SliderCard<int> alarma3Minuto(dashboard, "Alarma 3 - Minuto", 0, 59, 1, "min");
dash::SliderCard<int> alarma3Duracion(dashboard, "Alarma 3 - Duracion", 1, 300, 1, "seg");
dash::ToggleButtonCard alarma3Activa(dashboard, "Activar alarma 3");
dash::ToggleButtonCard alarma3Dom(dashboard, "A3 Domingo");
dash::ToggleButtonCard alarma3Lun(dashboard, "A3 Lunes");
dash::ToggleButtonCard alarma3Mar(dashboard, "A3 Martes");
dash::ToggleButtonCard alarma3Mie(dashboard, "A3 Miercoles");
dash::ToggleButtonCard alarma3Jue(dashboard, "A3 Jueves");
dash::ToggleButtonCard alarma3Vie(dashboard, "A3 Viernes");
dash::ToggleButtonCard alarma3Sab(dashboard, "A3 Sabado");

dash::SeparatorCard<dash::string> seccionControlAlarmas(dashboard, "Control general", "Acciones de alarmas");
dash::ToggleButtonCard btnFrenarAlarma(dashboard, "Frenar alarmas");

// Otras opciones
dash::SeparatorCard<dash::string> seccionOtrasOpciones(dashboard, "Otras opciones", "Datos adicionales del display");
dash::ToggleButtonCard btnMostrarFecha(dashboard, "Mostrar fecha");
dash::ToggleButtonCard btnMostrarTemperatura(dashboard, "Mostrar temperatura");
dash::ToggleButtonCard btnMostrarPresion(dashboard, "Mostrar presion");
dash::ToggleButtonCard btnMostrarExtrasAhora(dashboard, "Mostrar datos ahora");

// Recordatorios
dash::SeparatorCard<dash::string> seccionRecordatorios(dashboard, "Recordatorios", "Frases fijas en pantalla");
dash::ToggleButtonCard btnRecTarea(dashboard, "TAREA");
dash::ToggleButtonCard btnRecPerro(dashboard, "PERRO");
dash::ToggleButtonCard btnRecPlantas(dashboard, "PLANTAS");
dash::ToggleButtonCard btnRecCompras(dashboard, "COMPRAS");
dash::ToggleButtonCard btnRecPersonalizado(dashboard, "Texto propio");
dash::ToggleButtonCard btnBorrarRecordatorio(dashboard, "Borrar recordatorio");

void setup() {
  Serial.begin(115200);

  Wire.begin(BMP_SDA_PIN, BMP_SCL_PIN);
  SPI.begin(MATRIX_CLK_PIN, -1, MATRIX_DIN_PIN, MATRIX_CS_PIN);

  matriz.begin();
  matriz.setIntensity(5);
  matriz.displayClear();

  irsend.begin();

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BOTON_PIN, INPUT); // Pull-down externo
  pinMode(RELE_PIN, OUTPUT);
  digitalWrite(RELE_PIN, RELE_OFF);

  tone(BUZZER_PIN, 1000, 1);
  delay(1);
  noTone(BUZZER_PIN);

  matriz.displayText("Iniciando...", PA_CENTER, 80, 1000, PA_SCROLL_LEFT, PA_SCROLL_LEFT);

  if (!bmp.begin()) {
    matriz.displayClear();
    matriz.print("Error BMP180");
    while (1);
  }

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("IP ESP-DASH: ");
  Serial.println(WiFi.localIP());

  configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);

  registrarCallbacksLampara();
  registrarCallbacksAlarmas();
  registrarCallbacksDisplay();
  registrarCallbacksRecordatorios();

  cargarValoresInicialesAlarmas();
  cargarValoresInicialesDisplay();
  cargarValoresInicialesRecordatorios();

  dashboard.sendUpdates();

  server.begin();

  matriz.displayClear();

  Serial.println("ESP-DASH iniciado");
}

void loop() {
  controlarBotonFisico();
  actualizarDisplay();
}
