#include <WiFi.h>
#include <time.h>
#include <Wire.h>
#include <Adafruit_BMP085_U.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPDash.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

// WiFi
const char* ssid = "Wokwi-GUEST";
const char* password = "";
// MAX7219
#define HARDWARE_TYPE MD_MAX72XX::PAROLA_HW
#define MAX_DEVICES 4
#define CS_PIN 5
MD_Parola matriz = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
// BMP180
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);
// IR
#define IR_PIN 13
IRsend irsend(IR_PIN);
// BUZZER / ALARMA
#define BUZZER_PIN 12
int alarmaHora = 0;
int alarmaMinuto = 0;
int alarmaDuracion = 0;
bool alarmaActiva = false;
bool alarmaSonando = false;
bool alarmaYaDisparoHoy = false;
unsigned long inicioAlarma = 0;
// Hora Argentina
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -3 * 3600;
const int daylightOffset_sec = 0;
char mensaje[120];
// ESP-DASH
AsyncWebServer server(80);
ESPDash dashboard(server);
dash::SeparatorCard seccionLamparaRGB(
  dashboard,
  "Lampara RGB",
  "Control por infrarrojo"
);
// Botones ESP-DASH
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
// ==========================
// SECCION ALARMA
// ==========================

dash::SeparatorCard seccionAlarma(
  dashboard,
  "Alarma",
  "Control del buzzer"
);

dash::SliderCard<int> inputHoraAlarma(
  dashboard,
  "Hora alarma",
  0,
  23,
  1,
  "hs"
);

dash::SliderCard<int> inputMinutoAlarma(
  dashboard,
  "Minuto alarma",
  0,
  59,
  1,
  "min"
);

dash::SliderCard<int> inputDuracionAlarma(
  dashboard,
  "Duracion alarma",
  1,
  300,
  1,
  "seg"
);

dash::ToggleButtonCard btnActivarAlarma(
  dashboard,
  "Activar alarma"
);

dash::ToggleButtonCard btnFrenarAlarma(
  dashboard,
  "Frenar alarma"
);

// Función para enviar códigos IR
void enviarIR(uint32_t codigo) {
  irsend.sendNEC(codigo, 32);
  delay(100);
}
// Función para controlar alarma
void controlarAlarma(struct tm timeinfo) {
  if (!alarmaActiva) 
  {
    alarmaSonando = false;
    noTone(BUZZER_PIN);
    return;
  }
  // Reset diario a las 00:00
  if (timeinfo.tm_hour == 0 && timeinfo.tm_min == 0) {
    alarmaYaDisparoHoy = false;
  }
  // Disparo de alarma
  if (
    timeinfo.tm_hour == alarmaHora &&
    timeinfo.tm_min == alarmaMinuto &&
    !alarmaYaDisparoHoy
  ) {
    alarmaSonando = true;
    alarmaYaDisparoHoy = true;
    inicioAlarma = millis();
  }
  // Sonido de alarma
  if (alarmaSonando) {
    if (millis() - inicioAlarma <= alarmaDuracion * 1000UL) {
      tone(BUZZER_PIN, 2000);
    } else {
      alarmaSonando = false;
      noTone(BUZZER_PIN);
    }
  } else {
    noTone(BUZZER_PIN);
  }
}
void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);
  matriz.begin();
  matriz.setIntensity(5);
  matriz.displayClear();
  irsend.begin();
  pinMode(BUZZER_PIN, OUTPUT);

  tone (BUZZER_PIN, 1000,1);
  delay (1);
  noTone (BUZZER_PIN);


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
  Serial.println("");
  Serial.print("IP ESP-DASH: ");
  Serial.println(WiFi.localIP());
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  // ==========================
  // CALLBACKS LAMPARA RGB
  // ==========================
  btnOn.onChange([](bool state) {
    enviarIR(0x00F7C03F);
    btnOn.setValue(state);
    dashboard.sendUpdates();
    Serial.println("ENCENDIDO");
  });
  btnOff.onChange([](bool state) {
    enviarIR(0x00F740BF);
    btnOff.setValue(state);
    dashboard.sendUpdates();
  });
  btnBmas.onChange([](bool state) {
    enviarIR(0x00F700FF);
    btnBmas.setValue(state);
    dashboard.sendUpdates();
  });
  btnBmenos.onChange([](bool state) {
    enviarIR(0x00F7807F);
    btnBmenos.setValue(state);
    dashboard.sendUpdates();
  });
  // Colores principales
  btnR.onChange([](bool state) {
    enviarIR(0x00F720DF);
    btnR.setValue(state);
    dashboard.sendUpdates();
  });
  btnG.onChange([](bool state) {
    enviarIR(0x00F7A05F);
    btnG.setValue(state);
    dashboard.sendUpdates();
  });
  btnB.onChange([](bool state) {
    enviarIR(0x00F7609F);
    btnB.setValue(state);
    dashboard.sendUpdates();
  });
  btnW.onChange([](bool state) {
    enviarIR(0x00F7E01F);
    btnW.setValue(state);
    dashboard.sendUpdates();
  });
  // Modos
  btnFlash.onChange([](bool state) {
    enviarIR(0x00F7D02F);
    btnFlash.setValue(state);
    dashboard.sendUpdates();
  });
  btnStrobe.onChange([](bool state) {
    enviarIR(0x00F7F00F);
    btnStrobe.setValue(state);
    dashboard.sendUpdates();
  });
  btnFade.onChange([](bool state) {
    enviarIR(0x00F7C837);
    btnFade.setValue(state);
    dashboard.sendUpdates();
  });
  btnSmooth.onChange([](bool state) {
    enviarIR(0x00F7E817);
    btnSmooth.setValue(state);
    dashboard.sendUpdates();
  });
// ==========================
// CALLBACKS ALARMA
// ==========================

inputHoraAlarma.onChange([](int value) {

  alarmaHora = value;
  // DEBUG
  Serial.println (alarmaHora);
  inputHoraAlarma.setValue(alarmaHora);

  dashboard.sendUpdates();
});

inputMinutoAlarma.onChange([](int value) {

  alarmaMinuto = value;

  inputMinutoAlarma.setValue(alarmaMinuto);

  dashboard.sendUpdates();
});

inputDuracionAlarma.onChange([](int value) {

  alarmaDuracion = value;

  inputDuracionAlarma.setValue(alarmaDuracion);

  dashboard.sendUpdates();
});

btnActivarAlarma.onChange([](bool state) {

  alarmaActiva = state;

  btnActivarAlarma.setValue(state);

  if (!state) {

    alarmaSonando = false;

    noTone(BUZZER_PIN);
  }

  dashboard.sendUpdates();
});

btnFrenarAlarma.onChange([](bool state) {

  alarmaSonando = false;

  noTone(BUZZER_PIN);

  btnFrenarAlarma.setValue(false);

  dashboard.sendUpdates();
});

// inputHoraAlarma.setValue(alarmaHora);
  inputMinutoAlarma.setValue(alarmaMinuto);
  inputDuracionAlarma.setValue(alarmaDuracion);
  btnActivarAlarma.setValue(false);
  btnFrenarAlarma.setValue(false);

  dashboard.sendUpdates();

  server.begin();

  matriz.displayClear();

}

void loop() 
{
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
    controlarAlarma(timeinfo);
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
