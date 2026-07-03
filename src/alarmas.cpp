#include "alarmas.h"
#include "state.h"
#include "config.h"
#include "hardware.h"
#include "ui_cards.h"

void resetearMelodia() {
  noTone(BUZZER_PIN);
  notaAlarmaActual = 0;
  ultimoCambioNotaAlarma = 0;
}

void reproducirMelodiaAlarma() {
  if (millis() - ultimoCambioNotaAlarma >= (unsigned long)duracionMelodia[notaAlarmaActual]) {

    int frecuencia = melodiaAlarma[notaAlarmaActual];

    if (frecuencia == 0) {
      noTone(BUZZER_PIN);
    } else {
      tone(BUZZER_PIN, frecuencia);
    }

    ultimoCambioNotaAlarma = millis();

    notaAlarmaActual++;

    if (notaAlarmaActual >= cantidadNotasAlarma) {
      notaAlarmaActual = 0;
    }
  }
}

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
      if (millis() - alarmas[i].inicio <= (unsigned long)alarmas[i].duracion * 1000UL) {
        reproducirMelodiaAlarma();
      } else {
        alarmas[i].sonando = false;
        resetearMelodia();

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
    resetearMelodia();
  }
}

void frenarTodasLasAlarmas() {
  for (int i = 0; i < CANT_ALARMAS; i++) {
    alarmas[i].sonando = false;
  }

  resetearMelodia();
  Serial.println("Se han frenado todas las alarmas");
}

void registrarCallbacksAlarmas() {
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
    dashboard.sendUpdates();
  });
}

void cargarValoresInicialesAlarmas() {
  alarma1Hora.setValue(alarmas[0].hora);
  alarma1Minuto.setValue(alarmas[0].minuto);
  alarma1Duracion.setValue(alarmas[0].duracion);
  alarma1Activa.setValue(alarmas[0].activa);
  alarma1Dom.setValue(alarmas[0].dias[0]);
  alarma1Lun.setValue(alarmas[0].dias[1]);
  alarma1Mar.setValue(alarmas[0].dias[2]);
  alarma1Mie.setValue(alarmas[0].dias[3]);
  alarma1Jue.setValue(alarmas[0].dias[4]);
  alarma1Vie.setValue(alarmas[0].dias[5]);
  alarma1Sab.setValue(alarmas[0].dias[6]);

  alarma2Hora.setValue(alarmas[1].hora);
  alarma2Minuto.setValue(alarmas[1].minuto);
  alarma2Duracion.setValue(alarmas[1].duracion);
  alarma2Activa.setValue(alarmas[1].activa);
  alarma2Dom.setValue(alarmas[1].dias[0]);
  alarma2Lun.setValue(alarmas[1].dias[1]);
  alarma2Mar.setValue(alarmas[1].dias[2]);
  alarma2Mie.setValue(alarmas[1].dias[3]);
  alarma2Jue.setValue(alarmas[1].dias[4]);
  alarma2Vie.setValue(alarmas[1].dias[5]);
  alarma2Sab.setValue(alarmas[1].dias[6]);

  alarma3Hora.setValue(alarmas[2].hora);
  alarma3Minuto.setValue(alarmas[2].minuto);
  alarma3Duracion.setValue(alarmas[2].duracion);
  alarma3Activa.setValue(alarmas[2].activa);
  alarma3Dom.setValue(alarmas[2].dias[0]);
  alarma3Lun.setValue(alarmas[2].dias[1]);
  alarma3Mar.setValue(alarmas[2].dias[2]);
  alarma3Mie.setValue(alarmas[2].dias[3]);
  alarma3Jue.setValue(alarmas[2].dias[4]);
  alarma3Vie.setValue(alarmas[2].dias[5]);
  alarma3Sab.setValue(alarmas[2].dias[6]);

  btnFrenarAlarma.setValue(false);
}
