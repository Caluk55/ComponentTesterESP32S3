#include "test.h"
#include "tp.h"
#include "adc.h"
#include "display.h"
#include "config.h"

namespace test {

    void detectResistorTP1TP2() {
        tp::configureAsOutput(tp::TP1);
        tp::setLevel(tp::TP1, true);

        tp::configureAsInput(tp::TP2);

        delay(10);  // Stabilizzazione

        float v_out = adc::readVoltage(tp::TP2);

        if (v_out < VOLTAGE_THRESHOLD_LOW || v_out > VOLTAGE_THRESHOLD_HIGH) {
            display::clear();
            display::showMessage("Valore fuori range", TFT_RED);
            return;
        }

        float v_in = ADC_VREF;  // Tensione HIGH su TP1
        float r_fixed = TP_PULL_RESISTANCE;  // Resistenza verso massa

        // Calcolo partitore inverso
        float r_unknown = (v_out * r_fixed) / (v_in - v_out);

        display::clear();
        display::showMessage("Resistenza rilevata:", TFT_YELLOW);
        display::showMessage("TP1 ➝ TP2", TFT_GREEN);

        display::tft.setCursor(10, 100);
        if (r_unknown < 1000)
            display::tft.printf("%.0f Ohm", r_unknown);
        else
            display::tft.printf("%.2f kOhm", r_unknown / 1000.0f);
    }

}
