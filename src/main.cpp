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
#define CANT_ALARMAS 3

// BOTON FISICO
#define BOTON_PIN 2

bool displayEncendido = true;

bool botonEstadoAnterior = HIGH;
unsigned long tiempoPresionadoBoton = 0;
bool accionLargaEjecutada = false;

const unsigned long TIEMPO_PULSACION_LARGA = 1200; // ms

struct Alarma {
  int hora;
  int minuto;
  int duracion;
  bool activa;
  bool sonando;
  bool yaDisparoHoy;
  unsigned long inicio;
  bool dias[7]; 
};

Alarma alarmas[CANT_ALARMAS] = {
  {0, 0, 30, false, false, false, 0, {true, true, true, true, true, true, true}},
  {0, 0, 30, false, false, false, 0, {true, true, true, true, true, true, true}},
  {0, 0, 30, false, false, false, 0, {true, true, true, true, true, true, true}}
};


// ==========================
// OTRAS OPCIONES DISPLAY
// ==========================

const int intervaloDatosExtra = 40; // segundos

bool mostrarFechaActiva = true;
bool mostrarTemperaturaActiva = true;
bool mostrarPresionActiva = true;

bool mostrarExtrasAhora = false;

unsigned long ultimoExtraDisplay = 0;

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
// RECORDATORIOS
// ==========================

bool recordatorioActivo = false;
char fraseRecordatorio[20] = "";

char textoPersonalizado[] = "ESTUDIAR";

// ==========================
// SECCION GENERAL ALARMAS
// ==========================

dash::SeparatorCard seccionAlarmas(
  dashboard,
  "Alarmas",
  "Configuracion de alarmas"
);

// ==========================
// ALARMA 1
// ==========================

dash::SeparatorCard seccionAlarma1(
  dashboard,
  "Alarma 1",
  "Hora, minuto y duracion"
);

dash::SliderCard<int> alarma1Hora(
  dashboard,
  "Alarma 1 - Hora",
  0,
  23,
  1,
  "hs"
);

dash::SliderCard<int> alarma1Minuto(
  dashboard,
  "Alarma 1 - Minuto",
  0,
  59,
  1,
  "min"
);

dash::SliderCard<int> alarma1Duracion(
  dashboard,
  "Alarma 1 - Duracion",
  1,
  300,
  1,
  "seg"
);

dash::ToggleButtonCard alarma1Activa(
  dashboard,
  "Activar alarma 1"
);

dash::ToggleButtonCard alarma1Dom(dashboard, "Alarma 1 Domingo");
dash::ToggleButtonCard alarma1Lun(dashboard, "Alarma 1 Lunes");
dash::ToggleButtonCard alarma1Mar(dashboard, "Alarma 1 Martes");
dash::ToggleButtonCard alarma1Mie(dashboard, "Alarma 1 Miercoles");
dash::ToggleButtonCard alarma1Jue(dashboard, "Alarma 1 Jueves");
dash::ToggleButtonCard alarma1Vie(dashboard, "Alarma 1 Viernes");
dash::ToggleButtonCard alarma1Sab(dashboard, "Alarma 1 Sabado");

// ==========================
// ALARMA 2
// ==========================

dash::SeparatorCard seccionAlarma2(
  dashboard,
  "Alarma 2",
  "Hora, minuto y duracion"
);

dash::SliderCard<int> alarma2Hora(
  dashboard,
  "Alarma 2 - Hora",
  0,
  23,
  1,
  "hs"
);

dash::SliderCard<int> alarma2Minuto(
  dashboard,
  "Alarma 2 - Minuto",
  0,
  59,
  1,
  "min"
);

dash::SliderCard<int> alarma2Duracion(
  dashboard,
  "Alarma 2 - Duracion",
  1,
  300,
  1,
  "seg"
);

dash::ToggleButtonCard alarma2Activa(
  dashboard,
  "Activar alarma 2"
);

dash::ToggleButtonCard alarma2Dom(dashboard, "Alarma 2 Domingo");
dash::ToggleButtonCard alarma2Lun(dashboard, "Alarma 2 Lunes");
dash::ToggleButtonCard alarma2Mar(dashboard, "Alarma 2 Martes");
dash::ToggleButtonCard alarma2Mie(dashboard, "Alarma 2 Miercoles");
dash::ToggleButtonCard alarma2Jue(dashboard, "Alarma 2 Jueves");
dash::ToggleButtonCard alarma2Vie(dashboard, "Alarma 2 Viernes");
dash::ToggleButtonCard alarma2Sab(dashboard, "Alarma 2 Sabado");

// ==========================
// ALARMA 3
// ==========================

dash::SeparatorCard seccionAlarma3(
  dashboard,
  "Alarma 3",
  "Hora, minuto y duracion"
);

dash::SliderCard<int> alarma3Hora(
  dashboard,
  "Alarma 3 - Hora",
  0,
  23,
  1,
  "hs"
);

dash::SliderCard<int> alarma3Minuto(
  dashboard,
  "Alarma 3 - Minuto",
  0,
  59,
  1,
  "min"
);

dash::SliderCard<int> alarma3Duracion(
  dashboard,
  "Alarma 3 - Duracion",
  1,
  300,
  1,
  "seg"
);

dash::ToggleButtonCard alarma3Activa(
  dashboard,
  "Activar alarma 3"
);

dash::ToggleButtonCard alarma3Dom(dashboard, "Alarma 3 Domingo");
dash::ToggleButtonCard alarma3Lun(dashboard, "Alarma 3 Lunes");
dash::ToggleButtonCard alarma3Mar(dashboard, "Alarma 3 Martes");
dash::ToggleButtonCard alarma3Mie(dashboard, "Alarma 3 Miercoles");
dash::ToggleButtonCard alarma3Jue(dashboard, "Alarma 3 Jueves");
dash::ToggleButtonCard alarma3Vie(dashboard, "Alarma 3 Viernes");
dash::ToggleButtonCard alarma3Sab(dashboard, "Alarma 3 Sabado");

// ==========================
// CONTROL GENERAL ALARMAS
// ==========================

dash::SeparatorCard seccionControlAlarmas(
  dashboard,
  "Control general",
  "Acciones de alarmas"
);

dash::ToggleButtonCard btnFrenarAlarma(
  dashboard,
  "Frenar alarmas"
);

// ==========================
// OTRAS OPCIONES
// ==========================

dash::SeparatorCard seccionOtrasOpciones(
  dashboard,
  "Otras opciones",
  "Datos adicionales del display"
);

dash::ToggleButtonCard btnMostrarFecha(
  dashboard,
  "Mostrar fecha"
);

dash::ToggleButtonCard btnMostrarTemperatura(
  dashboard,
  "Mostrar temperatura"
);

dash::ToggleButtonCard btnMostrarPresion(
  dashboard,
  "Mostrar presion"
);

dash::ToggleButtonCard btnMostrarExtrasAhora(
  dashboard,
  "Mostrar datos ahora"
);

// Función para enviar códigos IR
void enviarIR(uint32_t codigo) {
  irsend.sendNEC(codigo, 32);
  delay(100);
}

// ==========================
// RECORDATORIOS
// ==========================

dash::SeparatorCard seccionRecordatorios(
  dashboard,
  "Recordatorios",
  "Frases fijas en pantalla"
);

dash::ToggleButtonCard btnRecTarea(
  dashboard,
  "TAREA"
);

dash::ToggleButtonCard btnRecPerro(
  dashboard,
  "PERRO"
);

dash::ToggleButtonCard btnRecPlantas(
  dashboard,
  "PLANTAS"
);

dash::ToggleButtonCard btnRecCompras(
  dashboard,
  "COMPRAS"
);

dash::ToggleButtonCard btnRecPersonalizado(
  dashboard,
  "Texto propio"
);

dash::ToggleButtonCard btnBorrarRecordatorio(
  dashboard,
  "Borrar recordatorio"
);

// Función para controlar alarma

void controlarAlarmas(struct tm timeinfo) {
  if (timeinfo.tm_hour == 0 && timeinfo.tm_min == 0) {
    for (int i = 0; i < CANT_ALARMAS; i++) {
      alarmas[i].yaDisparoHoy = false;
    }
  }

  for (int i = 0; i < CANT_ALARMAS; i++) {
    if (
      alarmas[i].activa &&
      alarmas[i].dias[timeinfo.tm_wday] &&
      !alarmas[i].yaDisparoHoy &&
      timeinfo.tm_hour == alarmas[i].hora &&
      timeinfo.tm_min == alarmas[i].minuto
    ) {
      alarmas[i].sonando = true;
      alarmas[i].yaDisparoHoy = true;
      alarmas[i].inicio = millis();

      Serial.print("Esta sonando la alarma ");
      Serial.println(i + 1);
    }

    if (alarmas[i].sonando) {
      if (millis() - alarmas[i].inicio <= alarmas[i].duracion * 1000UL) {
        tone(BUZZER_PIN, 2000);
      } else {
        alarmas[i].sonando = false;
        noTone(BUZZER_PIN);

        Serial.print("Finalizo la alarma ");
        Serial.println(i + 1);
      }
    }
  }
}

void imprimirAlarmaSerial(int numero) {
  struct tm timeinfo;

  Serial.print("Alarma ");
  Serial.print(numero + 1);

  if (getLocalTime(&timeinfo)) {
    Serial.print(" configurada a las ");
    if (timeinfo.tm_hour < 10) Serial.print("0");
    Serial.print(timeinfo.tm_hour);
    Serial.print(":");
    if (timeinfo.tm_min < 10) Serial.print("0");
    Serial.print(timeinfo.tm_min);
  }

  Serial.print(" para sonar a las ");
  if (alarmas[numero].hora < 10) Serial.print("0");
  Serial.print(alarmas[numero].hora);
  Serial.print(":");
  if (alarmas[numero].minuto < 10) Serial.print("0");
  Serial.print(alarmas[numero].minuto);

  Serial.print(" durante ");
  Serial.print(alarmas[numero].duracion);
  Serial.println(" segundos");
}

void activarDesactivarAlarma(int numero, bool estado) {
  alarmas[numero].activa = estado;

  if (estado) {
    Serial.print("Se ha activado la alarma ");
    Serial.println(numero + 1);
  } else {
    Serial.print("Se ha desactivado la alarma ");
    Serial.println(numero + 1);

    alarmas[numero].sonando = false;
    noTone(BUZZER_PIN);
  }
}

void frenarTodasLasAlarmas() {
  for (int i = 0; i < CANT_ALARMAS; i++) {
    alarmas[i].sonando = false;
  }

  noTone(BUZZER_PIN);
  Serial.println("Se han frenado todas las alarmas");
}

void activarRecordatorio(const char* texto) {
  recordatorioActivo = true;

  strncpy(fraseRecordatorio, texto, sizeof(fraseRecordatorio) - 1);
  fraseRecordatorio[sizeof(fraseRecordatorio) - 1] = '\0';

  btnRecTarea.setValue(false);
  btnRecPerro.setValue(false);
  btnRecPlantas.setValue(false);
  btnRecCompras.setValue(false);
  btnRecPersonalizado.setValue(false);
  btnBorrarRecordatorio.setValue(false);

  Serial.print("Recordatorio activo: ");
  Serial.println(fraseRecordatorio);
}

void setup() 
{
  Serial.begin(115200);
  Wire.begin(21, 22);
  matriz.begin();
  matriz.setIntensity(5);
  matriz.displayClear();
  irsend.begin();
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BOTON_PIN, INPUT);

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
// CALLBACKS ALARMAS
// ==========================

// ALARMA 1
alarma1Hora.onChange([](int value) {
  alarmas[0].hora = value;
  imprimirAlarmaSerial(0);
});

alarma1Minuto.onChange([](int value) {
  alarmas[0].minuto = value;
  imprimirAlarmaSerial(0);
});

alarma1Duracion.onChange([](int value) {
  alarmas[0].duracion = value;
  imprimirAlarmaSerial(0);
});

alarma1Activa.onChange([](bool state) {
  alarma1Activa.setValue(state);
  activarDesactivarAlarma(0, state);
  dashboard.sendUpdates();
});

// ALARMA 2
alarma2Hora.onChange([](int value) {
  alarmas[1].hora = value;
  imprimirAlarmaSerial(1);
});

alarma2Minuto.onChange([](int value) {
  alarmas[1].minuto = value;
  imprimirAlarmaSerial(1);
});

alarma2Duracion.onChange([](int value) {
  alarmas[1].duracion = value;
  imprimirAlarmaSerial(1);
});

alarma2Activa.onChange([](bool state) {
  alarma2Activa.setValue(state);
  activarDesactivarAlarma(1, state);
  dashboard.sendUpdates();
});

// ALARMA 3
alarma3Hora.onChange([](int value) {
  alarmas[2].hora = value;
  imprimirAlarmaSerial(2);
});

alarma3Minuto.onChange([](int value) {
  alarmas[2].minuto = value;
  imprimirAlarmaSerial(2);
});

alarma3Duracion.onChange([](int value) {
  alarmas[2].duracion = value;
  imprimirAlarmaSerial(2);
});

alarma3Activa.onChange([](bool state) {
  alarma3Activa.setValue(state);
  activarDesactivarAlarma(2, state);
  dashboard.sendUpdates();
});

// DIAS ALARMA 1
alarma1Dom.onChange([](bool state) { alarmas[0].dias[0] = state; alarma1Dom.setValue(state); dashboard.sendUpdates(); });
alarma1Lun.onChange([](bool state) { alarmas[0].dias[1] = state; alarma1Lun.setValue(state); dashboard.sendUpdates(); });
alarma1Mar.onChange([](bool state) { alarmas[0].dias[2] = state; alarma1Mar.setValue(state); dashboard.sendUpdates(); });
alarma1Mie.onChange([](bool state) { alarmas[0].dias[3] = state; alarma1Mie.setValue(state); dashboard.sendUpdates(); });
alarma1Jue.onChange([](bool state) { alarmas[0].dias[4] = state; alarma1Jue.setValue(state); dashboard.sendUpdates(); });
alarma1Vie.onChange([](bool state) { alarmas[0].dias[5] = state; alarma1Vie.setValue(state); dashboard.sendUpdates(); });
alarma1Sab.onChange([](bool state) { alarmas[0].dias[6] = state; alarma1Sab.setValue(state); dashboard.sendUpdates(); });

// DIAS ALARMA 2
alarma2Dom.onChange([](bool state) { alarmas[1].dias[0] = state; alarma2Dom.setValue(state); dashboard.sendUpdates(); });
alarma2Lun.onChange([](bool state) { alarmas[1].dias[1] = state; alarma2Lun.setValue(state); dashboard.sendUpdates(); });
alarma2Mar.onChange([](bool state) { alarmas[1].dias[2] = state; alarma2Mar.setValue(state); dashboard.sendUpdates(); });
alarma2Mie.onChange([](bool state) { alarmas[1].dias[3] = state; alarma2Mie.setValue(state); dashboard.sendUpdates(); });
alarma2Jue.onChange([](bool state) { alarmas[1].dias[4] = state; alarma2Jue.setValue(state); dashboard.sendUpdates(); });
alarma2Vie.onChange([](bool state) { alarmas[1].dias[5] = state; alarma2Vie.setValue(state); dashboard.sendUpdates(); });
alarma2Sab.onChange([](bool state) { alarmas[1].dias[6] = state; alarma2Sab.setValue(state); dashboard.sendUpdates(); });

// DIAS ALARMA 3
alarma3Dom.onChange([](bool state) { alarmas[2].dias[0] = state; alarma3Dom.setValue(state); dashboard.sendUpdates(); });
alarma3Lun.onChange([](bool state) { alarmas[2].dias[1] = state; alarma3Lun.setValue(state); dashboard.sendUpdates(); });
alarma3Mar.onChange([](bool state) { alarmas[2].dias[2] = state; alarma3Mar.setValue(state); dashboard.sendUpdates(); });
alarma3Mie.onChange([](bool state) { alarmas[2].dias[3] = state; alarma3Mie.setValue(state); dashboard.sendUpdates(); });
alarma3Jue.onChange([](bool state) { alarmas[2].dias[4] = state; alarma3Jue.setValue(state); dashboard.sendUpdates(); });
alarma3Vie.onChange([](bool state) { alarmas[2].dias[5] = state; alarma3Vie.setValue(state); dashboard.sendUpdates(); });
alarma3Sab.onChange([](bool state) { alarmas[2].dias[6] = state; alarma3Sab.setValue(state); dashboard.sendUpdates(); });

// FRENAR TODAS
btnFrenarAlarma.onChange([](bool state) {
  frenarTodasLasAlarmas();

  btnFrenarAlarma.setValue(false);
  alarma1Dom.setValue(alarmas[0].dias[0]);
alarma1Lun.setValue(alarmas[0].dias[1]);
alarma1Mar.setValue(alarmas[0].dias[2]);
alarma1Mie.setValue(alarmas[0].dias[3]);
alarma1Jue.setValue(alarmas[0].dias[4]);
alarma1Vie.setValue(alarmas[0].dias[5]);
alarma1Sab.setValue(alarmas[0].dias[6]);

alarma2Dom.setValue(alarmas[1].dias[0]);
alarma2Lun.setValue(alarmas[1].dias[1]);
alarma2Mar.setValue(alarmas[1].dias[2]);
alarma2Mie.setValue(alarmas[1].dias[3]);
alarma2Jue.setValue(alarmas[1].dias[4]);
alarma2Vie.setValue(alarmas[1].dias[5]);
alarma2Sab.setValue(alarmas[1].dias[6]);

alarma3Dom.setValue(alarmas[2].dias[0]);
alarma3Lun.setValue(alarmas[2].dias[1]);
alarma3Mar.setValue(alarmas[2].dias[2]);
alarma3Mie.setValue(alarmas[2].dias[3]);
alarma3Jue.setValue(alarmas[2].dias[4]);
alarma3Vie.setValue(alarmas[2].dias[5]);
alarma3Sab.setValue(alarmas[2].dias[6]);
  dashboard.sendUpdates();
});

// ==========================
// CALLBACKS OTRAS OPCIONES
// ==========================

btnMostrarFecha.onChange([](bool state) {
  mostrarFechaActiva = state;
  btnMostrarFecha.setValue(state);
  dashboard.sendUpdates();
});

btnMostrarTemperatura.onChange([](bool state) {
  mostrarTemperaturaActiva = state;
  btnMostrarTemperatura.setValue(state);
  dashboard.sendUpdates();
});

btnMostrarPresion.onChange([](bool state) {
  mostrarPresionActiva = state;
  btnMostrarPresion.setValue(state);
  dashboard.sendUpdates();
});

btnMostrarExtrasAhora.onChange([](bool state) {
  mostrarExtrasAhora = true;
  ultimoExtraDisplay = millis();

  btnMostrarExtrasAhora.setValue(false);
  
  alarma1Hora.setValue(alarmas[0].hora);
alarma1Minuto.setValue(alarmas[0].minuto);
alarma1Duracion.setValue(alarmas[0].duracion);
alarma1Activa.setValue(alarmas[0].activa);

alarma2Hora.setValue(alarmas[1].hora);
alarma2Minuto.setValue(alarmas[1].minuto);
alarma2Duracion.setValue(alarmas[1].duracion);
alarma2Activa.setValue(alarmas[1].activa);

alarma3Hora.setValue(alarmas[2].hora);
alarma3Minuto.setValue(alarmas[2].minuto);
alarma3Duracion.setValue(alarmas[2].duracion);
alarma3Activa.setValue(alarmas[2].activa);

btnFrenarAlarma.setValue(false);

  dashboard.sendUpdates();
});

btnMostrarFecha.setValue(mostrarFechaActiva);
btnMostrarTemperatura.setValue(mostrarTemperaturaActiva);
btnMostrarPresion.setValue(mostrarPresionActiva);
btnMostrarExtrasAhora.setValue(false);
btnRecTarea.setValue(false);
btnRecPerro.setValue(false);
btnRecPlantas.setValue(false);
btnRecCompras.setValue(false);
btnRecPersonalizado.setValue(false);
btnBorrarRecordatorio.setValue(false);

dashboard.sendUpdates();

// ==========================
// CALLBACKS RECORDATORIOS
// ==========================

btnRecTarea.onChange([](bool state) {
  if (state) {
    activarRecordatorio("TAREA");
    btnRecTarea.setValue(true);
  }
  dashboard.sendUpdates();
});

btnRecPerro.onChange([](bool state) {
  if (state) {
    activarRecordatorio("PERRO");
    btnRecPerro.setValue(true);
  }
  dashboard.sendUpdates();
});

btnRecPlantas.onChange([](bool state) {
  if (state) {
    activarRecordatorio("PLANT");
    btnRecPlantas.setValue(true);
  }
  dashboard.sendUpdates();
});

btnRecCompras.onChange([](bool state) {
  if (state) {
    activarRecordatorio("COMPRA");
    btnRecCompras.setValue(true);
  }
  dashboard.sendUpdates();
});

btnRecPersonalizado.onChange([](bool state) {
  if (state) {
    activarRecordatorio(textoPersonalizado);
    btnRecPersonalizado.setValue(true);
  }
  dashboard.sendUpdates();
});

btnBorrarRecordatorio.onChange([](bool state) {
  recordatorioActivo = false;
  strcpy(fraseRecordatorio, "");

  btnRecTarea.setValue(false);
  btnRecPerro.setValue(false);
  btnRecPlantas.setValue(false);
  btnRecCompras.setValue(false);
  btnRecPersonalizado.setValue(false);
  btnBorrarRecordatorio.setValue(false);

  Serial.println("Recordatorio borrado");

  dashboard.sendUpdates();
});

server.begin();
matriz.displayClear();
Serial.println("ESP-DASH iniciado");

}

void loop()
{
  struct tm timeinfo;

  float temperatura;
  bmp.getTemperature(&temperatura);

  float presion;
  bmp.getPressure(&presion);

if (recordatorioActivo) {
  matriz.displayClear();
  matriz.setTextAlignment(PA_CENTER);
  matriz.print(fraseRecordatorio);
  delay(500);
  return;
}

  bool usarScroll = false;

  if (!getLocalTime(&timeinfo)) {

    sprintf(mensaje, "Hora no disponible");
    usarScroll = true;

  } else {

    controlarAlarmas(timeinfo);

    unsigned long ahora = millis();

    bool mostrarExtras = false;

    if (ahora - ultimoExtraDisplay >= intervaloDatosExtra * 1000UL) {
      mostrarExtras = true;
      ultimoExtraDisplay = ahora;
    }

    if (mostrarExtrasAhora) {
      mostrarExtras = true;
      mostrarExtrasAhora = false;
      ultimoExtraDisplay = ahora;
    }

    strcpy(mensaje, "");

    sprintf(
      mensaje + strlen(mensaje),
      "%02d:%02d",
      timeinfo.tm_hour,
      timeinfo.tm_min
    );

    if (mostrarExtras) {

      if (mostrarFechaActiva) {
        sprintf(
          mensaje + strlen(mensaje),
          "  %02d/%02d/%04d",
          timeinfo.tm_mday,
          timeinfo.tm_mon + 1,
          timeinfo.tm_year + 1900
        );
        usarScroll = true;
      }

      if (mostrarTemperaturaActiva) {
        sprintf(
          mensaje + strlen(mensaje),
          "  Temp %.1f C",
          temperatura
        );
        usarScroll = true;
      }

      if (mostrarPresionActiva) {
        sprintf(
          mensaje + strlen(mensaje),
          "  Presion %.0f hPa",
          presion
        );
        usarScroll = true;
      }
    }
  }

  matriz.displayClear();

  if (usarScroll) {

    matriz.displayText(
      mensaje,
      PA_CENTER,
      80,
      1000,
      PA_SCROLL_LEFT,
      PA_SCROLL_LEFT
    );

    while (!matriz.displayAnimate()) {
      delay(10);
    }

  } else {

    matriz.setTextAlignment(PA_CENTER);
    matriz.print(mensaje);
    delay(500);
  }
}