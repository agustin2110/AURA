#pragma once

#include "hardware.h"

// Lampara RGB
extern dash::SeparatorCard<dash::string> seccionLamparaRGB;
extern dash::ToggleButtonCard btnOn;
extern dash::ToggleButtonCard btnOff;
extern dash::ToggleButtonCard btnBmas;
extern dash::ToggleButtonCard btnBmenos;
extern dash::ToggleButtonCard btnR;
extern dash::ToggleButtonCard btnG;
extern dash::ToggleButtonCard btnB;
extern dash::ToggleButtonCard btnW;
extern dash::ToggleButtonCard btnFlash;
extern dash::ToggleButtonCard btnStrobe;
extern dash::ToggleButtonCard btnFade;
extern dash::ToggleButtonCard btnSmooth;

// Alarmas
extern dash::SeparatorCard<dash::string> seccionAlarmas;

extern dash::SeparatorCard<dash::string> seccionAlarma1;
extern dash::SliderCard<int> alarma1Hora;
extern dash::SliderCard<int> alarma1Minuto;
extern dash::SliderCard<int> alarma1Duracion;
extern dash::ToggleButtonCard alarma1Activa;
extern dash::ToggleButtonCard alarma1Dom;
extern dash::ToggleButtonCard alarma1Lun;
extern dash::ToggleButtonCard alarma1Mar;
extern dash::ToggleButtonCard alarma1Mie;
extern dash::ToggleButtonCard alarma1Jue;
extern dash::ToggleButtonCard alarma1Vie;
extern dash::ToggleButtonCard alarma1Sab;

extern dash::SeparatorCard<dash::string> seccionAlarma2;
extern dash::SliderCard<int> alarma2Hora;
extern dash::SliderCard<int> alarma2Minuto;
extern dash::SliderCard<int> alarma2Duracion;
extern dash::ToggleButtonCard alarma2Activa;
extern dash::ToggleButtonCard alarma2Dom;
extern dash::ToggleButtonCard alarma2Lun;
extern dash::ToggleButtonCard alarma2Mar;
extern dash::ToggleButtonCard alarma2Mie;
extern dash::ToggleButtonCard alarma2Jue;
extern dash::ToggleButtonCard alarma2Vie;
extern dash::ToggleButtonCard alarma2Sab;

extern dash::SeparatorCard<dash::string> seccionAlarma3;
extern dash::SliderCard<int> alarma3Hora;
extern dash::SliderCard<int> alarma3Minuto;
extern dash::SliderCard<int> alarma3Duracion;
extern dash::ToggleButtonCard alarma3Activa;
extern dash::ToggleButtonCard alarma3Dom;
extern dash::ToggleButtonCard alarma3Lun;
extern dash::ToggleButtonCard alarma3Mar;
extern dash::ToggleButtonCard alarma3Mie;
extern dash::ToggleButtonCard alarma3Jue;
extern dash::ToggleButtonCard alarma3Vie;
extern dash::ToggleButtonCard alarma3Sab;

extern dash::SeparatorCard<dash::string> seccionControlAlarmas;
extern dash::ToggleButtonCard btnFrenarAlarma;

// Otras opciones
extern dash::SeparatorCard<dash::string> seccionOtrasOpciones;
extern dash::ToggleButtonCard btnMostrarFecha;
extern dash::ToggleButtonCard btnMostrarTemperatura;
extern dash::ToggleButtonCard btnMostrarPresion;
extern dash::ToggleButtonCard btnMostrarExtrasAhora;

// Recordatorios
extern dash::SeparatorCard<dash::string> seccionRecordatorios;
extern dash::ToggleButtonCard btnRecTarea;
extern dash::ToggleButtonCard btnRecPerro;
extern dash::ToggleButtonCard btnRecPlantas;
extern dash::ToggleButtonCard btnRecCompras;
extern dash::ToggleButtonCard btnRecPersonalizado;
extern dash::ToggleButtonCard btnBorrarRecordatorio;
