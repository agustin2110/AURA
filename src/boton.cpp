#include "boton.h"
#include "config.h"
#include "state.h"
#include "hardware.h"
#include "alarmas.h"
#include "recordatorios.h"
#include "ir_lampara.h"

void controlarBotonFisico() {
  bool botonEstado = digitalRead(BOTON_PIN);

  // Detecta cuando se presiona
  if (botonEstadoAnterior == LOW && botonEstado == HIGH) {
    tiempoPresionadoBoton = millis();
    accionLargaEjecutada = false;
    Serial.println("Boton presionado");
  }

  // Pulsacion larga
  if (botonEstado == HIGH && !accionLargaEjecutada) {
    if (millis() - tiempoPresionadoBoton >= TIEMPO_PULSACION_LARGA) {
      frenarTodasLasAlarmas();
      borrarRecordatorio();

      dashboard.sendUpdates();

      Serial.println("Pulsacion larga: alarmas frenadas y recordatorio borrado");

      accionLargaEjecutada = true;
    }
  }

  // Detecta cuando se suelta
  if (botonEstadoAnterior == HIGH && botonEstado == LOW) {
    unsigned long duracionPulsacion = millis() - tiempoPresionadoBoton;

    Serial.println("Boton soltado");

    if (duracionPulsacion < TIEMPO_PULSACION_LARGA && !accionLargaEjecutada) {
      if (lamparaEncendida) {
        apagarLampara();
        Serial.println("Pulsacion corta: lampara apagada");
      } else {
        encenderLampara();
        Serial.println("Pulsacion corta: lampara encendida");
      }
    }
  }

  botonEstadoAnterior = botonEstado;
}
