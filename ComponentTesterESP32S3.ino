// VERSIONE ATTUALE: aggiornata con initADC, initDisplay, initProbes, ecc.
// Vedi anche moduli: ADC.cpp, display.cpp, commands.cpp

#include <TFT_eSPI.h>
#include <Arduino.h>
// Moduli core
#include "config.h"
#include "variables.h"
#include "display.h"
#include "commands.h"
#include "user.h"
#include "adc.h"
#include "PWM.h"
#include "probes.h"
#include "tp.h

TFT_eSPI tft = TFT_eSPI();  // Oggetto globale del display

void setup() {
  Serial.begin(115200);
  delay(100);

  tp.begin()
  initConfig();
  initDisplay(&tft);
  showSplash();

  initADC();
  initPWM();
  initProbes();
  

  initCommands();
  initUser();
}

void loop() {
  commandsLoop();  // Gestione automatica
  userLoop();      // Eventuale interazione manuale
  tp.runTestCycle()
}
