#pragma once
#include <Arduino.h>
#include <time.h>

void reproducirMelodiaAlarma();
void controlarAlarmas(struct tm timeinfo);
void imprimirAlarmaSerial(int numero);
void activarDesactivarAlarma(int numero, bool estado);
void frenarTodasLasAlarmas();
void registrarCallbacksAlarmas();
void cargarValoresInicialesAlarmas();
