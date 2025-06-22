#include <Arduino.h>
#include <SPI.h>         // *** AGGIUNGI O ASSICURATI CHE SIA PRESENTE ***
#include <LovyanGFX.hpp> // Assicurati che questo sia incluso
// Dichiarazione esterna dell'istanza tft.
// Questo dice al compilatore che 'tft' esiste ed è definita altrove (es. LGFX_Config.hpp).
//extern LGFX tft; 

#include "config.h"
#include "display.h" // Questo includerà il nuovo display.h con LovyanGFX
#include "adc.h"
#include "tp.h"
#include "test.h"


void setup() {
  Serial.begin(115200);
  delay(100);

  display::init(); // Questa chiamata ora userà LovyanGFX
  tp::init();
  adc::init();
  display::showWelcomeScreen(); // Questa mostrerà il welcome screen con LovyanGFX
  delay(5000);    
}

// Nel tuo file .ino (o dove si trova la funzione loop())

void loop() {
  // Leggi e mostra le tensioni sui test point
  float v1 = adc::readVoltage(TEST_POINT_1);
  float v2 = adc::readVoltage(TEST_POINT_2);
  float v3 = adc::readVoltage(TEST_POINT_3);

  display::clear(); // Pulisce lo schermo prima di mostrare le tensioni
  // Ho rimosso l'uso di tft.fontHeight() qui.
  // Invece, ho usato un valore fisso (20) come spaziatura approssimativa,
  // perché la logica di layout dovrebbe essere gestita internamente da display::showVoltage
  // o i valori Y dovrebbero essere precalcolati/costanti.
  display::showVoltage(v1, "TP1", 10); 
  display::showVoltage(v2, "TP2", 10 + 20 + 5); // Coordinate fisse per TP2: 10 (offset iniziale) + 20 (altezza riga) + 5 (spazio)
  display::showVoltage(v3, "TP3", 10 + (2 * (20 + 5))); // Coordinate fisse per TP3: 10 + 2 * (altezza riga + spazio)

  delay(1000); // Rimane per 1 secondo

  // --- TEST COMPONENTI IN SEQUENZA ---

  // Resistenze
  display::clear(); // Pulisce lo schermo per il risultato della resistenza
  test::detectResistorTP(TEST_POINT_1, TEST_POINT_2);
  delay(1000);

  // Diodi (ogni misurazione di diodo, se distinta, avrà la sua schermata)
  display::clear(); // Pulisce lo schermo per il primo test diodi
  test::detectDiodeTP(TEST_POINT_1, TEST_POINT_2);
  delay(1000);

  display::clear(); // Pulisce lo schermo per il secondo test diodi
  test::detectDiodeTP(TEST_POINT_1, TEST_POINT_2);
  delay(1000);

  display::clear(); // Pulisce lo schermo per il terzo test diodi
  test::detectDiodeTP(TEST_POINT_2, TEST_POINT_3);
  delay(1000);

  display::clear(); // Pulisce lo schermo per il quarto test diodi
  test::detectDiodeTP(TEST_POINT_3, TEST_POINT_1);
  delay(1000);

  // Doppio diodo / LED
  display::clear(); // Pulisce lo schermo per il primo test doppio diodo
  test::detectDoubleDiodeOrLED(TEST_POINT_1, TEST_POINT_2, TEST_POINT_3);
  delay(1000);

  display::clear(); // Pulisce lo schermo per il secondo test doppio diodo
  test::detectDoubleDiodeOrLED(TEST_POINT_2, TEST_POINT_3, TEST_POINT_1);
  delay(1000);

  display::clear(); // Pulisce lo schermo per il terzo test doppio diodo
  test::detectDoubleDiodeOrLED(TEST_POINT_3, TEST_POINT_1, TEST_POINT_2);
  delay(1000);

  // BJT (transistor bipolari)
  display::clear(); // Pulisce lo schermo per il test BJT
  test::detectBJT();
  delay(1000);

  // MOSFET
  display::clear(); // Pulisce lo schermo per il test MOSFET
  test::detectMOSFET();
  delay(1000);

  // JFET
  display::clear(); // Pulisce lo schermo per il test JFET
  test::detectJFET();
  delay(1000);

  // IGBT
  display::clear(); // Pulisce lo schermo per il test IGBT
  test::detectIGBT();
  delay(1000);

  // SCR
  display::clear(); // Pulisce lo schermo per il test SCR
  test::detectSCR();
  delay(1000);

  // Triac
  display::clear(); // Pulisce lo schermo per il test Triac
  test::detectTriac();
  delay(1000);

  // PUT
  display::clear(); // Pulisce lo schermo per il test PUT
  test::detectPUT();
  delay(1000);

  // UJT
  display::clear(); // Pulisce lo schermo per il test UJT
  test::detectUJT();
  delay(1000);

  // Zener
  display::clear(); // Pulisce lo schermo per il test Zener
  test::detectZenerDiodes();
  delay(1000);

  // Quartz crystal
  display::clear(); // Pulisce lo schermo per il test Quartz
  test::detectQuartzCrystal();
  delay(1000);

  // OneWire device (es. DS18B20)
  display::clear(); // Pulisce lo schermo per il test OneWire
  test::detectOneWireDevice();
  delay(1000);

  // Induttore
  display::clear(); // Pulisce lo schermo per il test Induttore
  test::detectInductors();
  delay(1000);

  // Condensatore
  display::clear(); // Pulisce lo schermo per il test Condensatore
  test::detectCapacitors();
  delay(1000);

  // Se 'test::runAll()' è una funzione che a sua volta gestisce la pulizia
  // e il layout, allora non sarà necessario un 'display::clear()' qui.
  // Altrimenti, se 'runAll' stampa tutto in sequenza, potrebbe essere utile
  // un 'display::clear()' prima di chiamarla.
  // test::runAll();

  // Attendi prima di ripetere il ciclo
  delay(2000);
}