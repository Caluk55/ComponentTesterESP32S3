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

// Diodi 
void detectDiodes();
void detectDiodeTP(TP a, TP b);

// Doppi Diodi
void detectDoubleDiodeOrLED(TP commonPin, TP anodePin, TP cathodePin); // Nuova dichiarazione

// Zener
void detectZenerDiodes();
void detectZenerTP(TP a, TP b);

// Induttori
void detectInductors();
void detectInductorTP(TP a, TP b);


// Condensatori + ESR
void detectCapacitors();
void detectCapacitorTP(TP a, TP b);
void detectESR(TP a, TP b);   // <--- AGGIUNTA

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
