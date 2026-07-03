#include "state.h"

Alarma alarmas[CANT_ALARMAS] = {
  {0, 0, 30, false, false, false, 0, {true, true, true, true, true, true, true}},
  {0, 0, 30, false, false, false, 0, {true, true, true, true, true, true, true}},
  {0, 0, 30, false, false, false, 0, {true, true, true, true, true, true, true}}
};

bool lamparaEncendida = false;
bool botonEstadoAnterior = LOW;
unsigned long tiempoPresionadoBoton = 0;
bool accionLargaEjecutada = false;

const int intervaloDatosExtra = 40;
bool mostrarFechaActiva = true;
bool mostrarTemperaturaActiva = true;
bool mostrarPresionActiva = true;
bool mostrarExtrasAhora = false;
unsigned long ultimoExtraDisplay = 0;
char mensaje[140];

bool recordatorioActivo = false;
char fraseRecordatorio[20] = "";
char textoPersonalizado[] = "PAGO";

const int melodiaAlarma[] = {
  880, 0, 880, 0,
  988, 0, 988, 0,
  1175, 0, 988, 0,
  880, 0, 784, 0
};

const int duracionMelodia[] = {
  120, 80, 120, 120,
  120, 80, 120, 120,
  180, 80, 180, 80,
  180, 80, 250, 200
};

const int cantidadNotasAlarma = sizeof(melodiaAlarma) / sizeof(melodiaAlarma[0]);

int notaAlarmaActual = 0;
unsigned long ultimoCambioNotaAlarma = 0;
