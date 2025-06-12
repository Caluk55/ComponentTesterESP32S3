#include "tp.h"
#include <Arduino.h>

// Array di mappatura TP <-> Pin fisico
static const int TP_PINS[3] = { TP1_PIN, TP2_PIN, TP3_PIN };

namespace tp {

void init() {
    for (int i = 0; i < 3; ++i) {
        pinMode(TP_PINS[i], INPUT);
        // Se vuoi abilitare pull-down hardware (se supportato), fallo qui
        // pinMode(TP_PINS[i], INPUT_PULLDOWN); // Solo se ESP32S3 lo supporta
    }
}

void setMode(TP tp, uint8_t mode) {
    pinMode(TP_PINS[tp], mode);
}

void write(TP tp, uint8_t value) {
    digitalWrite(TP_PINS[tp], value);
}

int readDigital(TP tp) {
    return digitalRead(TP_PINS[tp]);
}

void floatAll() {
    for (int i = 0; i < 3; ++i) {
        pinMode(TP_PINS[i], INPUT);
    }
}

int tpToPin(TP tp) {
    return TP_PINS[tp];
}

} // namespace tp