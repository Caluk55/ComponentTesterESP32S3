#include "tp.h"

// Mappa dei pin fisici su ESP32-S3 per TP1–TP3
constexpr uint8_t TP_PINS[3] = { 4, 5, 6 };  // Modifica secondo il tuo schema

namespace tp {

    void begin() {
        for (uint8_t i = 0; i < 3; ++i) {
            pinMode(TP_PINS[i], INPUT);
        }
    }

    void configureAsInput(TPLabel tp) {
        pinMode(TP_PINS[tp], INPUT);
    }

    void configureAsOutput(TPLabel tp) {
        pinMode(TP_PINS[tp], OUTPUT);
    }

    void setLevel(TPLabel tp, bool level) {
        digitalWrite(TP_PINS[tp], level ? HIGH : LOW);
    }

    int readLevel(TPLabel tp) {
        return digitalRead(TP_PINS[tp]);
    }

    void runTestCycle() {
        // Stub: logica di test da implementare
        // Esempio: alterna TP1 output HIGH/LOW e legge TP2
    }

}
