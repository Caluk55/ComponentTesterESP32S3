#include <Arduino.h>
#include "config.h"
#include "display.h"
#include "adc.h"
#include "tp.h"
#include "test.h"

void setup() {
  Serial.begin(115200);
  delay(100);

  initDisplay();
  tp::init();
  adc::init();
  display::showWelcomeScreen();
}

void loop() {
  // Leggi e mostra le tensioni sui test point
  float v1 = adc::readVoltage(TP1);
  float v2 = adc::readVoltage(TP2);
  float v3 = adc::readVoltage(TP3);

  display::clear();
  display::showVoltage(v1, "TP1");
  display::showVoltage(v2, "TP2");
  display::showVoltage(v3, "TP3");

  delay(1000);

  // --- TEST COMPONENTI IN SEQUENZA ---

  // Resistenze
  test::detectResistorTP1TP2();
  delay(1000);

  // Diodi
  test::detectDiodeTP1TP2();
  delay(1000);
  test::detectDiodeBetween(tp::TP1, tp::TP2);
  delay(1000);
  test::detectDiodeBetween(tp::TP2, tp::TP3);
  delay(1000);
  test::detectDiodeBetween(tp::TP3, tp::TP1);
  delay(1000);

  // Doppio diodo / LED
  test::detectDoubleDiodeOrLED(tp::TP1, tp::TP2);
  delay(1000);
  test::detectDoubleDiodeOrLED(tp::TP2, tp::TP3);
  delay(1000);
  test::detectDoubleDiodeOrLED(tp::TP3, tp::TP1);
  delay(1000);

  // BJT (transistor bipolari)
  test::detectBJT();
  delay(1000);

  // MOSFET
  test::detectMOSFET();
  delay(1000);

  // JFET
  test::detectJFET();
  delay(1000);

  // IGBT
  test::detectIGBT();
  delay(1000);

  // SCR
  test::detectSCR();
  delay(1000);

  // Triac
  test::detectTriac();
  delay(1000);

  // PUT
  test::detectPUT();
  delay(1000);

  // UJT
  test::detectUJT();
  delay(1000);

  // Zener
  test::detectZenerDiode();
  delay(1000);

  // Quartz crystal
  test::detectQuartzCrystal();
  delay(1000);

  // OneWire device (es. DS18B20)
  test::detectOneWireDevice();
  delay(1000);

  // Induttore
  test::detectInductor();
  delay(1000);

  // Condensatore
  test::detectCapacitor();
  delay(1000);

  // Per test multipli o esecuzione batch, puoi anche chiamare:
  // test::runAll();

  // Attendi prima di ripetere il ciclo
  delay(2000);
}