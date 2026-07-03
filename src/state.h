#pragma once

#include <Arduino.h>
#include "config.h"

// ==========================
// ALARMAS
// ==========================
#define CANT_ALARMAS 3

struct Alarma {
  int hora;
  int minuto;
  int duracion;
  bool activa;
  bool sonando;
  bool yaDisparoHoy;
  unsigned long inicio;
  bool dias[7]; // 0 Dom, 1 Lun, 2 Mar, 3 Mie, 4 Jue, 5 Vie, 6 Sab
};

extern Alarma alarmas[CANT_ALARMAS];

// ==========================
// LAMPARA / BOTON
// ==========================
extern bool lamparaEncendida;
extern bool botonEstadoAnterior;
extern unsigned long tiempoPresionadoBoton;
extern bool accionLargaEjecutada;

// ==========================
// DISPLAY
// ==========================
extern const int intervaloDatosExtra;
extern bool mostrarFechaActiva;
extern bool mostrarTemperaturaActiva;
extern bool mostrarPresionActiva;
extern bool mostrarExtrasAhora;
extern unsigned long ultimoExtraDisplay;
extern char mensaje[140];

// ==========================
// RECORDATORIOS
// ==========================
extern bool recordatorioActivo;
extern char fraseRecordatorio[20];
extern char textoPersonalizado[];

// ==========================
// MELODIA
// ==========================
extern const int melodiaAlarma[];
extern const int duracionMelodia[];
extern const int cantidadNotasAlarma;
extern int notaAlarmaActual;
extern unsigned long ultimoCambioNotaAlarma;
