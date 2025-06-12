#ifndef TEST_H
#define TEST_H

#include "config.h"
#include "tp.h" // <--- AGGIUNGI QUESTO

namespace test {

// Inizializzazione generale test
void init();

// Resistenze
void detectResistors();
void detectResistorTP(TP a, TP b);

// Diodi/LED
void detectDiodes();
void detectDiodeTP(TP a, TP b);

// Zener
void detectZenerDiodes();
void detectZenerTP(TP a, TP b);

// Induttori
void detectInductors();
void detectInductorTP(TP a, TP b);

// Condensatori
void detectCapacitors();
void detectCapacitorTP(TP a, TP b);

// Transistor BJT
void detectBJT();

// MOSFET
void detectMOSFET();

// JFET
void detectJFET();

// IGBT
void detectIGBT();

// SCR
void detectSCR();

// Triac
void detectTriac();

// PUT
void detectPUT();

// UJT
void detectUJT();

// Quarzo
void detectQuartzCrystal();

// OneWire
void detectOneWireDevice();

// Zener
void detectZenerDiodes();
void detectZenerTP(TP a, TP b, int gpio_stepup);

// Led
void detectLEDs();
void detectLEDTP(TP a, TP b, int gpio_stepup);

// Esegue tutti i test in sequenza
void runAll();

} // namespace test

#endif // TEST_H
