#ifndef TP_H
#define TP_H

#include "config.h"
#include <stdint.h>

// Definizione tipo TP (Test Point)
typedef uint8_t TP;

// Alias numerici per i test point (modifica secondo il tuo hardware in config.h se serve)
#define TEST_POINT_1 0
#define TEST_POINT_2 1
#define TEST_POINT_3 2

namespace tp {

// Inizializza i Test Point (imposta i pin come input, eventuali pull-down)
void init();

// Imposta la modalità di un TP (INPUT/OUTPUT)
void setMode(TP tp, uint8_t mode);

// Scrive HIGH/LOW su un TP
void write(TP tp, uint8_t value);

// Legge lo stato digitale di un TP (HIGH/LOW)
int readDigital(TP tp);

// (Opzionale) Isola tutti i TP (INPUT senza pull, per alta impedenza)
void floatAll();

// Utility: converte TP in pin fisico (es. TP1 -> pin 4)
int tpToPin(TP tp);

} // namespace tp

#endif // TP_H
