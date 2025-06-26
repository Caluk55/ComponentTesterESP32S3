#include "tp.h"
#include <Arduino.h>

// Array di mappatura TP <-> Pin fisico
static const int TP_PINS[3] = { TP1_PIN, TP2_PIN, TP3_PIN };

// Array di mappatura GPIO per le resistenze 470k e 680Ω
static const int TP_470K_PINS[3] = { TP1_470K_PIN, TP2_470K_PIN, TP3_470K_PIN };
static const int TP_680_PINS[3]  = { TP1_680_PIN,  TP2_680_PIN,  TP3_680_PIN  };

namespace tp {

void init() {
    for (int i = 0; i < 3; ++i) {
        pinMode(TP_PINS[i], INPUT);
        pinMode(TP_470K_PINS[i], OUTPUT);
        pinMode(TP_680_PINS[i], OUTPUT);
        digitalWrite(TP_470K_PINS[i], LOW); // Resistenze disabilitate all'avvio
        digitalWrite(TP_680_PINS[i], LOW);
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

// --- Gestione Resistenze Commutate ---

void set470k(TP tp, bool enable) {
    digitalWrite(TP_470K_PINS[tp], enable ? HIGH : LOW);
}

void set680(TP tp, bool enable) {
    digitalWrite(TP_680_PINS[tp], enable ? HIGH : LOW);
}

void disableAllResistors() {
    for (int i = 0; i < 3; ++i) {
        digitalWrite(TP_470K_PINS[i], LOW);
        digitalWrite(TP_680_PINS[i], LOW);
    }
}

} // namespace tp