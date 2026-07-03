#include "recordatorios.h"
#include "state.h"
#include "hardware.h"
#include "ui_cards.h"

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

void borrarRecordatorio() {
  recordatorioActivo = false;
  strcpy(fraseRecordatorio, "");

  btnRecTarea.setValue(false);
  btnRecPerro.setValue(false);
  btnRecPlantas.setValue(false);
  btnRecCompras.setValue(false);
  btnRecPersonalizado.setValue(false);
  btnBorrarRecordatorio.setValue(false);

  Serial.println("Recordatorio borrado");
}

void registrarCallbacksRecordatorios() {
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
    borrarRecordatorio();
    dashboard.sendUpdates();
  });
}

void cargarValoresInicialesRecordatorios() {
  btnRecTarea.setValue(false);
  btnRecPerro.setValue(false);
  btnRecPlantas.setValue(false);
  btnRecCompras.setValue(false);
  btnRecPersonalizado.setValue(false);
  btnBorrarRecordatorio.setValue(false);
}
