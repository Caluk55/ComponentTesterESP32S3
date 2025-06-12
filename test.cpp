#include "test.h"
#include "adc.h"
#include "display.h"
#include "tp.h"
#include "config.h"

namespace test {

// --- Utility di visualizzazione ---
namespace {
void showRes(float r) {
    if (r < 1000.0f) {
        char buf[16];
        snprintf(buf, sizeof(buf), "%.2f Ohm", r);
        display::showMessage(buf);
    } else if (r < 1e6f) {
        char buf[16];
        snprintf(buf, sizeof(buf), "%.2f kOhm", r / 1000.0f);
        display::showMessage(buf);
    } else {
        char buf[16];
        snprintf(buf, sizeof(buf), "%.2f MOhm", r / 1e6f);
        display::showMessage(buf);
    }
}

void showInd(float l) {
    if (l < 1e-6) {
        display::showMessage("Induttanza troppo bassa");
    } else if (l < 1e-3) {
        char buf[20];
        snprintf(buf, sizeof(buf), "%.2f uH", l * 1e6);
        display::showMessage(buf);
    } else if (l < 1) {
        char buf[20];
        snprintf(buf, sizeof(buf), "%.2f mH", l * 1e3);
        display::showMessage(buf);
    } else {
        char buf[20];
        snprintf(buf, sizeof(buf), "%.2f H", l);
        display::showMessage(buf);
    }
}
}

// --- Inizializzazione ---
void init() {
    // Eventuali inizializzazioni hardware
}

// --- 1. Resistenze ---
void detectResistorTP(TP a, TP b) {
    tp::floatAll();
    delay(5);
    tp::setMode(a, OUTPUT); 
    tp::write(a, HIGH);
    tp::setMode(b, OUTPUT); 
    tp::write(b, LOW);

    delay(5);
    float va = adc::readVoltage(a);
    float vb = adc::readVoltage(b);
    float v_drop = va - vb;
    float current = (ADC_VREF - va) / TP_SERIES_RESISTANCE;
    if (current < 1e-7)
        current = 1e-7;
    float r = v_drop / current - TP_SERIES_RESISTANCE;

    display::clear();
    display::showMessage("Resistenza tra TP:");
    showRes(r);
}
void detectResistors() {
    detectResistorTP(TEST_POINT_1, TEST_POINT_2); delay(500);
    detectResistorTP(TEST_POINT_2, TEST_POINT_3); delay(500);
    detectResistorTP(TEST_POINT_3, TEST_POINT_1); delay(500);
}

// --- 2. Diodi/LED ---
void detectDiodeTP(TP a, TP b) {
    tp::floatAll();
    delay(5);
    tp::setMode(a, OUTPUT); 
    tp::write(a, HIGH);
    tp::setMode(b, OUTPUT); 
    tp::write(b, LOW);
    delay(5);
    float v_fwd = adc::readVoltage(a) - adc::readVoltage(b);
    tp::write(a, LOW); 
    tp::write(b, HIGH);
    delay(5);
    float v_rev = adc::readVoltage(b) - adc::readVoltage(a);

    display::clear();
    if (v_fwd > 0.25f && v_fwd < 2.5f && v_rev < 0.2f) {
        display::showMessage("Diodo rilevato:");
        display::showVoltage(v_fwd, "Vf");
    } else {
        display::showMessage("Nessun diodo");
    }
}
void detectDiodes() {
    detectDiodeTP(TEST_POINT_1, TEST_POINT_2); delay(500);
    detectDiodeTP(TEST_POINT_2, TEST_POINT_3); delay(500);
    detectDiodeTP(TEST_POINT_3, TEST_POINT_1); delay(500);
}

// --- 3. Zener ---
void detectZenerTP(TP a, TP b) {
    tp::floatAll(); delay(5);
    tp::setMode(a, OUTPUT); 
    tp::write(a, LOW);
    tp::setMode(b, OUTPUT); 
    tp::write(b, HIGH);
    delay(10);
    float v_zener = adc::readVoltage(a) - adc::readVoltage(b);

    display::clear();
    if (v_zener > 2.0f && v_zener < 24.0f) {
        display::showMessage("Zener rilevato:");
        display::showVoltage(v_zener, "Vz");
    } else {
        display::showMessage("Nessun Zener");
    }
}
void detectZenerDiodes() {
    detectZenerTP(TEST_POINT_1, TEST_POINT_2); delay(500);
    detectZenerTP(TEST_POINT_2, TEST_POINT_3); delay(500);
    detectZenerTP(TEST_POINT_3, TEST_POINT_1); delay(500);
}

// --- 4. Induttori ---
void detectInductorTP(TP a, TP b) {
    tp::floatAll();
    delay(5);

    tp::setMode(a, OUTPUT); 
    tp::write(a, LOW);
    tp::setMode(b, OUTPUT); 
    tp::write(b, LOW);
    delay(10);

    tp::setMode(a, OUTPUT); 
    tp::write(a, HIGH);
    tp::setMode(b, INPUT);
    delayMicroseconds(100);

    float v_peak = adc::readVoltage(b);

    float dt = 100e-6f;
    float dI = ADC_VREF / TP_SERIES_RESISTANCE;
    float L = (v_peak * dt) / dI;

    display::clear();
    display::showMessage("Induttanza TP:");
    showInd(L);
}
void detectInductors() {
    detectInductorTP(TEST_POINT_1, TEST_POINT_2); delay(1000);
    detectInductorTP(TEST_POINT_2, TEST_POINT_3); delay(1000);
    detectInductorTP(TEST_POINT_3, TEST_POINT_1); delay(1000);
}

// --- 5. Condensatori ---
void detectCapacitorTP(TP a, TP b) {
    tp::floatAll(); delay(5);
    tp::setMode(a, OUTPUT); 
    tp::write(a, LOW);
    tp::setMode(b, OUTPUT); 
    tp::write(b, LOW);
    delay(50);

    tp::setMode(a, OUTPUT); 
    tp::write(a, HIGH);
    tp::setMode(b, INPUT);
    delay(2);

    unsigned long t0 = millis();
    float vb = 0;
    const float Vtarget = 0.632f * ADC_VREF;
    while (vb < Vtarget && millis() - t0 < 250) {
        vb = adc::readVoltage(b);
    }
    unsigned long t1 = millis();
    float dt = (float)(t1 - t0) / 1000.0f;
    float c = dt / TP_SERIES_RESISTANCE;

    display::clear();
    display::showMessage("Capacita' TP:");
    char buf[24];
    if (c < 1e-9)
        snprintf(buf, sizeof(buf), "%.2f pF", c * 1e12);
    else if (c < 1e-6)
        snprintf(buf, sizeof(buf), "%.2f nF", c * 1e9);
    else if (c < 1e-3)
        snprintf(buf, sizeof(buf), "%.2f uF", c * 1e6);
    else
        snprintf(buf, sizeof(buf), "%.2f mF", c * 1e3);
    display::showMessage(buf);
}
void detectCapacitors() {
    detectCapacitorTP(TEST_POINT_1, TEST_POINT_2); delay(1000);
    detectCapacitorTP(TEST_POINT_2, TEST_POINT_3); delay(1000);
    detectCapacitorTP(TEST_POINT_3, TEST_POINT_1); delay(1000);
}

// --- 6. BJT ---
void detectBJT() {
    struct BJTTest { TP e, b, c; } combos[6] = {
        {TEST_POINT_1, TEST_POINT_2, TEST_POINT_3}, {TEST_POINT_1, TEST_POINT_3, TEST_POINT_2},
        {TEST_POINT_2, TEST_POINT_1, TEST_POINT_3}, {TEST_POINT_2, TEST_POINT_3, TEST_POINT_1},
        {TEST_POINT_3, TEST_POINT_1, TEST_POINT_2}, {TEST_POINT_3, TEST_POINT_2, TEST_POINT_1}
    };
    for (int i = 0; i < 6; ++i) {
        TP e = combos[i].e, b = combos[i].b, c = combos[i].c;

        // NPN
        tp::floatAll();
        tp::setMode(b, OUTPUT); tp::write(b, HIGH);
        tp::setMode(e, OUTPUT); tp::write(e, LOW);
        tp::setMode(c, INPUT);
        delay(5);
        float v_be = adc::readVoltage(b) - adc::readVoltage(e);
        float v_bc = adc::readVoltage(b) - adc::readVoltage(c);
        if (v_be > 0.55 && v_be < 0.8 && v_bc > 0.55 && v_bc < 0.8) {
            display::clear();
            display::showMessage("BJT NPN rilevato");
            char buf[32];
            snprintf(buf, sizeof(buf), "E=%d B=%d C=%d", e+1, b+1, c+1);
            display::showMessage(buf);
            return;
        }
        // PNP
        tp::floatAll();
        tp::setMode(b, OUTPUT); tp::write(b, LOW);
        tp::setMode(e, OUTPUT); tp::write(e, HIGH);
        tp::setMode(c, INPUT);
        delay(5);
        v_be = adc::readVoltage(e) - adc::readVoltage(b);
        v_bc = adc::readVoltage(c) - adc::readVoltage(b);
        if (v_be > 0.55 && v_be < 0.8 && v_bc > 0.55 && v_bc < 0.8) {
            display::clear();
            display::showMessage("BJT PNP rilevato");
            char buf[32];
            snprintf(buf, sizeof(buf), "E=%d B=%d C=%d", e+1, b+1, c+1);
            display::showMessage(buf);
            return;
        }
    }
    display::clear();
    display::showMessage("Nessun BJT trovato");
}

// --- 7. MOSFET (N/P, enhancement) ---
void detectMOSFET() {
    tp::floatAll();
    tp::setMode(TEST_POINT_1, OUTPUT); tp::write(TEST_POINT_1, HIGH);
    tp::setMode(TEST_POINT_2, OUTPUT); tp::write(TEST_POINT_2, LOW);
    tp::setMode(TEST_POINT_3, INPUT);
    delay(5);
    float v_ds = adc::readVoltage(TEST_POINT_3) - adc::readVoltage(TEST_POINT_2);
    float v_gs = adc::readVoltage(TEST_POINT_1) - adc::readVoltage(TEST_POINT_2);
    if (v_ds < 0.1 && v_gs > 1.5) {
        display::clear();
        display::showMessage("N-MOSFET rilevato");
        display::showMessage("G=TEST_POINT_1 S=TEST_POINT_2 D=TEST_POINT_3");
        return;
    }
    display::clear();
    display::showMessage("Nessun MOSFET trovato");
}

// --- 8. JFET ---
void detectJFET() {
    display::clear();
    display::showMessage("Rilevamento JFET...");

    struct JFETTest { TP s, d, g; };
    const JFETTest combos[6] = {
        {TEST_POINT_1, TEST_POINT_2, TEST_POINT_3}, {TEST_POINT_1, TEST_POINT_3, TEST_POINT_2},
        {TEST_POINT_2, TEST_POINT_1, TEST_POINT_3}, {TEST_POINT_2, TEST_POINT_3, TEST_POINT_1},
        {TEST_POINT_3, TEST_POINT_1, TEST_POINT_2}, {TEST_POINT_3, TEST_POINT_2, TEST_POINT_1}
    };

    bool found = false;
    for (int i = 0; i < 6 && !found; ++i) {
        TP s = combos[i].s, d = combos[i].d, g = combos[i].g;

        tp::floatAll();
        tp::setMode(g, INPUT);
        tp::setMode(s, OUTPUT); tp::write(s, LOW);
        tp::setMode(d, OUTPUT); tp::write(d, HIGH);
        delay(10);

        float vs_f = adc::readVoltage(s);
        float vd_f = adc::readVoltage(d);
        float v_sd_f = vd_f - vs_f;

        tp::setMode(g, OUTPUT); tp::write(g, LOW);
        delay(10);
        float vs_n = adc::readVoltage(s);
        float vd_n = adc::readVoltage(d);
        float v_sd_n = vd_n - vs_n;

        tp::write(g, HIGH);
        delay(10);
        float vs_p = adc::readVoltage(s);
        float vd_p = adc::readVoltage(d);
        float v_sd_p = vd_p - vs_p;

        bool is_njfet = (v_sd_f < 0.5 && v_sd_n > 2.0 && v_sd_p < 0.5);
        bool is_pjfet = (v_sd_f < 0.5 && v_sd_n < 0.5 && v_sd_p > 2.0);

        if (is_njfet) {
            display::clear();
            display::showMessage("N-JFET rilevato");
            char buf[32];
            snprintf(buf, sizeof(buf), "S=%d D=%d G=%d", s+1, d+1, g+1);
            display::showMessage(buf);
            found = true;
        } else if (is_pjfet) {
            display::clear();
            display::showMessage("P-JFET rilevato");
            char buf[32];
            snprintf(buf, sizeof(buf), "S=%d D=%d G=%d", s+1, d+1, g+1);
            display::showMessage(buf);
            found = true;
        }
    }
    if (!found) {
        display::clear();
        display::showMessage("Nessun JFET trovato");
    }
}

// --- 9. IGBT ---
void detectIGBT() {
    display::clear();
    display::showMessage("Rilevamento IGBT...");

    struct IGBTTest { TP c, e, g; };
    const IGBTTest combos[6] = {
        {TEST_POINT_1, TEST_POINT_2, TEST_POINT_3}, {TEST_POINT_1, TEST_POINT_3, TEST_POINT_2},
        {TEST_POINT_2, TEST_POINT_1, TEST_POINT_3}, {TEST_POINT_2, TEST_POINT_3, TEST_POINT_1},
        {TEST_POINT_3, TEST_POINT_1, TEST_POINT_2}, {TEST_POINT_3, TEST_POINT_2, TEST_POINT_1}
    };

    bool found = false;
    for (int i = 0; i < 6 && !found; ++i) {
        TP c = combos[i].c, e = combos[i].e, g = combos[i].g;

        tp::floatAll();
        delay(10);

        tp::setMode(g, OUTPUT); tp::write(g, HIGH);
        tp::setMode(e, OUTPUT); tp::write(e, LOW);
        tp::setMode(c, INPUT);
        delay(10);

        tp::setMode(g, INPUT);

        float v_ce_before = adc::readVoltage(c) - adc::readVoltage(e);

        tp::setMode(g, OUTPUT); tp::write(g, HIGH);
        delay(10);
        tp::setMode(g, INPUT);
        float v_ce_after = adc::readVoltage(c) - adc::readVoltage(e);

        tp::setMode(c, OUTPUT); tp::write(c, LOW);
        tp::setMode(e, OUTPUT); tp::write(e, LOW);
        delay(10);
        tp::setMode(c, INPUT);

        float v_ce_conduct = adc::readVoltage(c) - adc::readVoltage(e);

        if (v_ce_before > 2.0 && v_ce_after < 0.5 && v_ce_conduct < 0.5) {
            display::clear();
            display::showMessage("IGBT rilevato!");
            char buf[32];
            snprintf(buf, sizeof(buf), "C=%d E=%d G=%d", c+1, e+1, g+1);
            display::showMessage(buf);
            found = true;
        }
    }

    if (!found) {
        display::clear();
        display::showMessage("Nessun IGBT trovato");
    }
}

// --- 10. SCR ---
void detectSCR() {
    display::clear();
    display::showMessage("Rilevamento SCR...");

    struct SCRTest { TP a, k, g; };
    const SCRTest combos[6] = {
        {TEST_POINT_1, TEST_POINT_2, TEST_POINT_3}, {TEST_POINT_1, TEST_POINT_3, TEST_POINT_2},
        {TEST_POINT_2, TEST_POINT_1, TEST_POINT_3}, {TEST_POINT_2, TEST_POINT_3, TEST_POINT_1},
        {TEST_POINT_3, TEST_POINT_1, TEST_POINT_2}, {TEST_POINT_3, TEST_POINT_2, TEST_POINT_1}
    };

    bool found = false;
    for (int i = 0; i < 6 && !found; ++i) {
        TP a = combos[i].a, k = combos[i].k, g = combos[i].g;

        tp::floatAll();
        delay(10);

        tp::setMode(a, OUTPUT); tp::write(a, HIGH);
        tp::setMode(k, OUTPUT); tp::write(k, LOW);
        tp::setMode(g, INPUT);
        delay(10);

        float v_ak_off = adc::readVoltage(a) - adc::readVoltage(k);

        tp::setMode(g, OUTPUT); tp::write(g, HIGH);
        delay(5);
        tp::write(g, LOW);
        tp::setMode(g, INPUT);
        delay(5);

        float v_ak_on = adc::readVoltage(a) - adc::readVoltage(k);

        if (v_ak_off > 2.0 && v_ak_on < 0.5) {
            display::clear();
            display::showMessage("SCR rilevato!");
            char buf[32];
            snprintf(buf, sizeof(buf), "A=%d K=%d G=%d", a+1, k+1, g+1);
            display::showMessage(buf);
            found = true;
        }
    }

    if (!found) {
        display::clear();
        display::showMessage("Nessun SCR trovato");
    }
}

// --- 11. Triac ---
void detectTriac() {
    display::clear();
    display::showMessage("Rilevamento Triac...");

    struct TriacTest { TP mt1, mt2, g; };
    const TriacTest combos[6] = {
        {TEST_POINT_1, TEST_POINT_2, TEST_POINT_3}, {TEST_POINT_1, TEST_POINT_3, TEST_POINT_2},
        {TEST_POINT_2, TEST_POINT_1, TEST_POINT_3}, {TEST_POINT_2, TEST_POINT_3, TEST_POINT_1},
        {TEST_POINT_3, TEST_POINT_1, TEST_POINT_2}, {TEST_POINT_3, TEST_POINT_2, TEST_POINT_1}
    };

    bool found = false;
    for (int i = 0; i < 6 && !found; ++i) {
        TP mt1 = combos[i].mt1, mt2 = combos[i].mt2, g = combos[i].g;

        tp::floatAll();
        delay(10);

        tp::setMode(mt1, OUTPUT); tp::write(mt1, LOW);
        tp::setMode(mt2, OUTPUT); tp::write(mt2, HIGH);
        tp::setMode(g, INPUT);
        delay(10);

        float v_mt12_off = adc::readVoltage(mt2) - adc::readVoltage(mt1);

        tp::setMode(g, OUTPUT); tp::write(g, HIGH);
        delay(5);
        tp::write(g, LOW);
        tp::setMode(g, INPUT);
        delay(5);

        float v_mt12_on = adc::readVoltage(mt2) - adc::readVoltage(mt1);

        if (v_mt12_off > 2.0 && v_mt12_on < 0.5) {
            display::clear();
            display::showMessage("Triac rilevato!");
            char buf[32];
            snprintf(buf, sizeof(buf), "MT1=%d MT2=%d G=%d", mt1+1, mt2+1, g+1);
            display::showMessage(buf);
            found = true;
        }
    }

    if (!found) {
        display::clear();
        display::showMessage("Nessun Triac trovato");
    }
}

// --- 12. PUT ---
void detectPUT() {
    display::clear();
    display::showMessage("Rilevamento PUT...");

    struct PUTTest { TP a, k, g; };
    const PUTTest combos[6] = {
        {TEST_POINT_1, TEST_POINT_2, TEST_POINT_3}, {TEST_POINT_1, TEST_POINT_3, TEST_POINT_2},
        {TEST_POINT_2, TEST_POINT_1, TEST_POINT_3}, {TEST_POINT_2, TEST_POINT_3, TEST_POINT_1},
        {TEST_POINT_3, TEST_POINT_1, TEST_POINT_2}, {TEST_POINT_3, TEST_POINT_2, TEST_POINT_1}
    };

    bool found = false;
    for (int i = 0; i < 6 && !found; ++i) {
        TP a = combos[i].a, k = combos[i].k, g = combos[i].g;

        tp::floatAll();
        delay(10);

        tp::setMode(a, OUTPUT); tp::write(a, HIGH);
        tp::setMode(k, OUTPUT); tp::write(k, LOW);
        tp::setMode(g, INPUT);
        delay(10);

        float v_ak_off = adc::readVoltage(a) - adc::readVoltage(k);

        tp::setMode(g, OUTPUT); tp::write(g, HIGH);
        delay(5);
        tp::write(g, LOW);
        tp::setMode(g, INPUT);
        delay(5);

        float v_ak_on = adc::readVoltage(a) - adc::readVoltage(k);

        if (v_ak_off > 2.0 && v_ak_on < 0.5) {
            display::clear();
            display::showMessage("PUT rilevato!");
            char buf[32];
            snprintf(buf, sizeof(buf), "A=%d K=%d G=%d", a+1, k+1, g+1);
            display::showMessage(buf);
            found = true;
        }
    }

    if (!found) {
        display::clear();
        display::showMessage("Nessun PUT trovato");
    }
}

// --- 13. UJT ---
void detectUJT() {
    display::clear();
    display::showMessage("Rilevamento UJT...");

    struct UJTTest { TP b1, b2, e; };
    const UJTTest combos[6] = {
        {TEST_POINT_1, TEST_POINT_2, TEST_POINT_3}, {TEST_POINT_1, TEST_POINT_3, TEST_POINT_2},
        {TEST_POINT_2, TEST_POINT_1, TEST_POINT_3}, {TEST_POINT_2, TEST_POINT_3, TEST_POINT_1},
        {TEST_POINT_3, TEST_POINT_1, TEST_POINT_2}, {TEST_POINT_3, TEST_POINT_2, TEST_POINT_1}
    };

    bool found = false;
    for (int i = 0; i < 6 && !found; ++i) {
        TP b1 = combos[i].b1, b2 = combos[i].b2, e = combos[i].e;

        tp::floatAll();
        delay(10);

        tp::setMode(b1, OUTPUT); tp::write(b1, HIGH);
        tp::setMode(b2, OUTPUT); tp::write(b2, LOW);
        tp::setMode(e, INPUT);
        delay(10);
        float v_b1b2 = adc::readVoltage(b1) - adc::readVoltage(b2);

        tp::setMode(e, OUTPUT); tp::write(e, HIGH);
        delay(5);
        float v_b1b2_after = adc::readVoltage(b1) - adc::readVoltage(b2);

        if (v_b1b2 > 1.0 && (v_b1b2 - v_b1b2_after) > 0.5) {
            display::clear();
            display::showMessage("UJT rilevato!");
            char buf[32];
            snprintf(buf, sizeof(buf), "B1=%d B2=%d E=%d", b1+1, b2+1, e+1);
            display::showMessage(buf);
            found = true;
        }
    }

    if (!found) {
        display::clear();
        display::showMessage("Nessun UJT trovato");
    }
}

// --- 14. Quarzo (Quartz Crystal) ---
void detectQuartzCrystal() {
    display::clear();
    display::showMessage("Test quarzo...");

    tp::floatAll();
    tp::setMode(TEST_POINT_1, OUTPUT);
    tp::setMode(TEST_POINT_2, OUTPUT);
    tp::setMode(TEST_POINT_3, INPUT);

    for (int i = 0; i < 500; ++i) {
        tp::write(TEST_POINT_1, HIGH);
        tp::write(TEST_POINT_2, LOW);
        delayMicroseconds(2);
        tp::write(TEST_POINT_1, LOW);
        tp::write(TEST_POINT_2, HIGH);
        delayMicroseconds(2);
    }

    float v_tp3 = adc::readVoltage(TEST_POINT_3);

    if (v_tp3 > 0.2 && v_tp3 < (ADC_VREF - 0.2)) {
        display::showMessage("Quarzo rilevato!");
    } else {
        display::showMessage("Nessun quarzo trovato");
    }
}

// --- 15. OneWire ---
void detectOneWireDevice() {
    display::clear();
    display::showMessage("Test OneWire...");

    tp::floatAll();
    tp::setMode(TEST_POINT_1, OUTPUT);
    tp::write(TEST_POINT_1, HIGH); // Pull-up
    delay(1);

    tp::write(TEST_POINT_1, LOW);
    delayMicroseconds(480);
    tp::write(TEST_POINT_1, HIGH);
    delayMicroseconds(70);

    tp::setMode(TEST_POINT_1, INPUT);
    float v = adc::readVoltage(TEST_POINT_1);
    delayMicroseconds(410);

    if (v < (ADC_VREF * 0.7)) {
        display::showMessage("OneWire trovato!");
    } else {
        display::showMessage("Nessun OneWire trovato");
    }
}

// --- 16. Zener (con step-up) ---
#define GPIO_STEPUP_EN  25

void detectZenerTP(TP a, TP b, int gpio_stepup) {
    pinMode(gpio_stepup, OUTPUT);
    digitalWrite(gpio_stepup, HIGH);
    delay(60);

    tp::floatAll();
    delay(5);
    tp::setMode(a, OUTPUT); tp::write(a, HIGH);
    tp::setMode(b, OUTPUT); tp::write(b, LOW);
    delay(30);

    float v_zener = adc::readVoltage(a) - adc::readVoltage(b);

    digitalWrite(gpio_stepup, LOW);

    display::clear();
    if (v_zener > 2.0f && v_zener < 35.0f) {
        display::showMessage("Zener rilevato:");
        display::showVoltage(v_zener, "Vz");
    } else {
        display::showMessage("Nessun Zener");
    }
}



// --- 17. LED (con step-up) ---
void detectLEDTP(TP a, TP b, int gpio_stepup) {
    pinMode(gpio_stepup, OUTPUT);
    digitalWrite(gpio_stepup, HIGH);
    delay(50);

    tp::floatAll();
    delay(5);
    tp::setMode(a, OUTPUT); tp::write(a, HIGH);
    tp::setMode(b, OUTPUT); tp::write(b, LOW);
    delay(15);

    float v_fwd = adc::readVoltage(a) - adc::readVoltage(b);

    digitalWrite(gpio_stepup, LOW);

    display::clear();
    if (v_fwd > 1.3f && v_fwd < 4.5f) {
        display::showMessage("LED rilevato:");
        display::showVoltage(v_fwd, "Vf");
    } else {
        display::showMessage("Nessun LED");
    }
}

// Nuova funzione per testare un doppio diodo a tre piedini
void detectDoubleDiodeOrLED(int pin1, int pin2, int pin3) {
    Serial.println("Test del doppio diodo (3 piedini)");
    
    // Legge lo stato dei tre piedini
    int state1 = digitalRead(pin1);
    int state2 = digitalRead(pin2);
    int state3 = digitalRead(pin3);
    
    // Esempio di logica (da adattare in base alle caratteristiche specifiche del componente):
    // Ad esempio, un doppio diodo a tre piedini potrebbe avere un terminale comune e due terminali separati.
    // Qui verifichiamo se due degli ingressi (es. pin1 e pin2) sono HIGH e il terzo (es. pin3) è LOW.
    
    if(state1 == HIGH && state2 == HIGH && state3 == LOW) {
        Serial.println("Doppio diodo rilevato: configurazione 1");
    }
    else if(state1 == LOW && state2 == HIGH && state3 == HIGH) {
        Serial.println("Doppio diodo rilevato: configurazione 2");
    }
    else if(state1 == HIGH && state2 == LOW && state3 == HIGH) {
        Serial.println("Doppio diodo rilevato: configurazione 3");
    }
    else {
        Serial.println("Doppio diodo non rilevato o configurazione non riconosciuta");
    }
}




void detectLEDs() {
    detectLEDTP(TEST_POINT_1, TEST_POINT_2, GPIO_STEPUP_EN); delay(500);
    detectLEDTP(TEST_POINT_2, TEST_POINT_3, GPIO_STEPUP_EN); delay(500);
    detectLEDTP(TEST_POINT_3, TEST_POINT_1, GPIO_STEPUP_EN); delay(500);
}

// --- Master: esegue tutto ---
void runAll() {
    detectResistors();
    detectDiodes();
    detectZenerDiodes();
    detectInductors();
    detectCapacitors();
    detectBJT();
    detectMOSFET();
    detectJFET();
    detectIGBT();
    detectSCR();
    detectTriac();
    detectPUT();
    detectUJT();
    detectQuartzCrystal();
    detectOneWireDevice();
}
} // namespace test