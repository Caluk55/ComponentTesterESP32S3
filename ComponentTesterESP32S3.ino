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
#include "test.h"

TFT_eSPI tft = TFT_eSPI();  // Oggetto globale del display

void setup() {
  Serial.begin(115200);
  delay(100);
  display::showWelcomeScreen();   // Mostra messaggio iniziale
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
  float v1 = adc::readVoltage(tp::TP1);
  float v2 = adc::readVoltage(tp::TP2);
  float v3 = adc::readVoltage(tp::TP3);

  display::clear();  // Pulisce lo schermo prima di aggiornare
  display::showVoltage(v1, "TP1");
  display::showVoltage(v2, "TP2");
  display::showVoltage(v3, "TP3");

  delay(1000);  // Aggiorna ogni secondo
  test::detectResistorTP1TP2();
  delay(3000);

}
