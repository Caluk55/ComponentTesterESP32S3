#ifndef ADC_H
#define ADC_H

#include "config.h"
#include "tp.h" // <--- AGGIUNGI QUESTO

namespace adc {

// Inizializzazione dell'ADC (se necessaria)
void init();

// Legge la tensione (in Volt) su un TP specifico (TP1, TP2, TP3)
float readVoltage(TP tp);

// Facoltativo: funzione per leggere la raw value (ADC grezzo)
uint16_t readRaw(TP tp);

} // namespace adc

#endif // ADC_H