#include <Arduino.h>
#include "config.h"
#include "display.h"
#include "adc.h"
#include "tp.h"
#include "test.h"

void setup() {
  Serial.begin(115200);
  delay(100);

 display::init(); // CORRETTO
  tp::init();
  adc::init();
  display::showWelcomeScreen();
}

void loop() {
  // Leggi e mostra le tensioni sui test point
  float v1 = adc::readVoltage(TEST_POINT_1);
    float v2 = adc::readVoltage(TEST_POINT_2);
    float v3 = adc::readVoltage(TEST_POINT_3);
  display::clear();
  display::showVoltage(v1, "TP1");
  display::showVoltage(v2, "TP2");
  display::showVoltage(v3, "TP3");

  delay(1000);

  // --- TEST COMPONENTI IN SEQUENZA ---

  // Resistenze
test::detectResistorTP(TEST_POINT_1, TEST_POINT_2);
  delay(1000);

  // Diodi
  test::detectDiodeTP(TEST_POINT_1, TEST_POINT_2);
  delay(1000);
  test::detectDiodeTP(TEST_POINT_1, TEST_POINT_2);
  delay(1000);
  test::detectDiodeTP(TEST_POINT_2, TEST_POINT_3);
  delay(1000);
  test::detectDiodeTP(TEST_POINT_3, TEST_POINT_1);
  delay(1000);

  // Doppio diodo / LED
  test::detectDoubleDiodeOrLED(TEST_POINT_1, TEST_POINT_2);
  delay(1000);
  test::detectDoubleDiodeOrLED(TEST_POINT_2, TEST_POINT_3);
  delay(1000);
  test::detectDoubleDiodeOrLED(TEST_POINT_3, TEST_POINT_1);
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