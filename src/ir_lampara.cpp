#include "ir_lampara.h"
#include "hardware.h"
#include "ui_cards.h"
#include "state.h"
#include "config.h"

void enviarIR(uint32_t codigo) {
  irsend.sendNEC(codigo, 32);
  delay(100);
}

void encenderLampara() {
  enviarIR(0x00F7C03F);      // ON IR
  digitalWrite(RELE_PIN, RELE_ON);

  lamparaEncendida = true;

  btnOn.setValue(true);
  btnOff.setValue(false);

  dashboard.sendUpdates();

  Serial.println("Lampara ENCENDIDA");
}

void apagarLampara() {
  enviarIR(0x00F740BF);      // OFF IR
  digitalWrite(RELE_PIN, RELE_OFF);

  lamparaEncendida = false;

  btnOff.setValue(true);
  btnOn.setValue(false);

  dashboard.sendUpdates();

  Serial.println("Lampara APAGADA");
}

void registrarCallbacksLampara() {
  btnOn.onChange([](bool state) {
    encenderLampara();
  });

  btnOff.onChange([](bool state) {
    apagarLampara();
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
}
