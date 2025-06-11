#include "adc.h"

// Mappa TP → pin analogici (modifica secondo il tuo schema)
constexpr uint8_t TP_ADC_PINS[3] = { TP1_PIN, TP2_PIN, TP3_PIN }; // Devono essere ADC-capable

namespace adc {

    void begin() {
        // Nessuna inizializzazione necessaria per analogRead() in Arduino
        // Ma puoi impostare attenuazione o calibrazione se necessario
    }

    float readVoltage(tp::TPLabel tpLabel) {
        uint8_t pin = TP_ADC_PINS[tpLabel];
        int raw = analogRead(pin);
        float voltage = (raw / ADC_RESOLUTION) * ADC_VREF;  // 12-bit ADC, 3.3V ref
        return voltage;
    }

    void calibrate() {
        // Stub: calibrazione futura con eFuse o Vref interno
    }

    void detectComponent() {
        // Stub: logica di identificazione (resistenza, diodo, ecc.)
    }

}
