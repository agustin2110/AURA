#include "display_control.h"
#include <time.h>
#include "state.h"
#include "hardware.h"
#include "ui_cards.h"
#include "alarmas.h"

void registrarCallbacksDisplay() {
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
    dashboard.sendUpdates();
  });
}

void cargarValoresInicialesDisplay() {
  btnMostrarFecha.setValue(mostrarFechaActiva);
  btnMostrarTemperatura.setValue(mostrarTemperaturaActiva);
  btnMostrarPresion.setValue(mostrarPresionActiva);
  btnMostrarExtrasAhora.setValue(false);
}

void actualizarDisplay() {
  struct tm timeinfo;

  float temperatura;
  bmp.getTemperature(&temperatura);

  float presion;
  bmp.getPressure(&presion);

  bool horaOk = getLocalTime(&timeinfo);

  if (horaOk) {
    controlarAlarmas(timeinfo);
  }

  if (recordatorioActivo) {
    matriz.displayClear();
    matriz.setTextAlignment(PA_CENTER);
    matriz.print(fraseRecordatorio);
    delay(500);
    return;
  }

  bool usarScroll = false;

  if (!horaOk) {
    sprintf(mensaje, "Hora no disponible");
    usarScroll = true;
  } else {
    unsigned long ahora = millis();

    bool mostrarExtras = false;

    if (ahora - ultimoExtraDisplay >= (unsigned long)intervaloDatosExtra * 1000UL) {
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
