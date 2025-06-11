#ifndef ADC_H
#define ADC_H

#include <Arduino.h>
#include "tp.h"

namespace adc {

    void begin();
    float readVoltage(tp::TPLabel tp);
    void calibrate();  // Stub per calibrazione futura
    void detectComponent();  // Stub per logica di identificazione

}

#endif // ADC_H
