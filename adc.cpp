#include "adc.h"
#include <Arduino.h>
#include "tp.h" // <--- AGGIUNGI QUESTO

// Mappa i TP ai rispettivi pin fisici
static const int TP_PINS[3] = { TP1_PIN, TP2_PIN, TP3_PIN };

namespace adc {

void init() {
    // Se serve configurare ADC, fallo qui (per ESP32S3 di solito non serve)
    // analogReadResolution(12); // Se vuoi forzare la risoluzione
}

uint16_t readRaw(TP tp) {
    int pin = TP_PINS[tp];
    return analogRead(pin);
}

float readVoltage(TP tp) {
    uint16_t raw = readRaw(tp);
    float voltage = (raw / ADC_RESOLUTION) * ADC_VREF;
    return voltage;
}

} // namespace adc