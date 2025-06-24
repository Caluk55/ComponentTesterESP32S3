#include "test.h"
#include "adc.h"
#include "display.h"
#include "tp.h"
#include "config.h"
#include "icons.h"

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

    // --- Nuova visualizzazione ---
    char bufR[32];
    if (r < 1e3)
        snprintf(bufR, sizeof(bufR), "R = %.2f Ohm", r);
    else if (r < 1e6)
        snprintf(bufR, sizeof(bufR), "R = %.2f kOhm", r / 1e3);
    else
        snprintf(bufR, sizeof(bufR), "R = %.2f MOhm", r / 1e6);

    char pinA[8], pinB[8];
    snprintf(pinA, sizeof(pinA), "TP%d", a+1);
    snprintf(pinB, sizeof(pinB), "TP%d", b+1);
    const char* pins[] = {pinA, pinB};
    const char* params[] = {bufR};
    showComponentResult(RESISTOR_SYM, "Resistenza", pins, 2, params, 1);
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

    char pinA[8], pinB[8];
    snprintf(pinA, sizeof(pinA), "TP%d", a+1);
    snprintf(pinB, sizeof(pinB), "TP%d", b+1);
    const char* pins[] = {pinA, pinB};

    if (v_fwd > 0.25f && v_fwd < 2.5f && v_rev < 0.2f) {
        // Diodo rilevato
        char bufVf[24];
        snprintf(bufVf, sizeof(bufVf), "Vf = %.2f V", v_fwd);
        const char* params[] = {bufVf};

        // Se Vf > 1.3V, probabilmente è un LED
        if (v_fwd > 1.3f)
            showComponentResult(LED_SYM, "LED", pins, 2, params, 1);
        else
            showComponentResult(DIODE_SYM, "Diodo", pins, 2, params, 1);
    } else {
        showComponentResult(DIODE_SYM, "Nessun diodo", pins, 2, nullptr, 0);
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

    // --- Nuova visualizzazione ---
    char bufVz[32];
    snprintf(bufVz, sizeof(bufVz), "Vz = %.2f V", v_zener);

    char pinA[8], pinB[8];
    snprintf(pinA, sizeof(pinA), "TP%d", a+1);
    snprintf(pinB, sizeof(pinB), "TP%d", b+1);
    const char* pins[] = {pinA, pinB};
    const char* params[] = {bufVz};

    if (v_zener > 2.0f && v_zener < 24.0f) {
        showComponentResult(ZENER_DIODE_SYM, "Zener", pins, 2, params, 1);
    } else {
        showComponentResult(ZENER_DIODE_SYM, "Nessun Zener", pins, 2, nullptr, 0);
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

    // --- Nuova visualizzazione ---
    char bufL[32];
    if (L < 1e-6)
        snprintf(bufL, sizeof(bufL), "L = %.2f uH", L * 1e6);
    else if (L < 1e-3)
        snprintf(bufL, sizeof(bufL), "L = %.2f mH", L * 1e3);
    else
        snprintf(bufL, sizeof(bufL), "L = %.2f H", L);

    char pinA[8], pinB[8];
    snprintf(pinA, sizeof(pinA), "TP%d", a+1);
    snprintf(pinB, sizeof(pinB), "TP%d", b+1);
    const char* pins[] = {pinA, pinB};
    const char* params[] = {bufL};
    showComponentResult(INDUCTOR_SYM, "Induttore", pins, 2, params, 1);
}
void detectInductors() {
    detectInductorTP(TEST_POINT_1, TEST_POINT_2); delay(1000);
    detectInductorTP(TEST_POINT_2, TEST_POINT_3); delay(1000);
    detectInductorTP(TEST_POINT_3, TEST_POINT_1); delay(1000);
}

// --- 5. Condensatori + ESR ---
void detectESR(TP a, TP b, float* esr_out = nullptr) {
    tp::floatAll();
    delay(3);

    // Scarica condensatore
    tp::setMode(a, OUTPUT); tp::write(a, LOW);
    tp::setMode(b, OUTPUT); tp::write(b, LOW);
    delay(10);

    // Applica impulso breve per misurare ESR
    tp::setMode(a, OUTPUT); tp::write(a, HIGH);
    tp::setMode(b, INPUT);
    delayMicroseconds(80); // impulso brevissimo

    float v_drop = adc::readVoltage(b);

    // Calcolo ESR (Ohm): ESR = Vdrop / Iimpulso
    float Iimpulso = (ADC_VREF - v_drop) / TP_SERIES_RESISTANCE;
    if (Iimpulso < 1e-6) Iimpulso = 1e-6;
    float esr = v_drop / Iimpulso;

    if (esr_out) *esr_out = esr;

    // --- Nuova visualizzazione ---
    char bufESR[20];
    if (esr < 1.0)
        snprintf(bufESR, sizeof(bufESR), "ESR = %.2f Ohm", esr);
    else
        snprintf(bufESR, sizeof(bufESR), "ESR = %.1f Ohm", esr);

    char pinA[8], pinB[8];
    snprintf(pinA, sizeof(pinA), "TP%d", a+1);
    snprintf(pinB, sizeof(pinB), "TP%d", b+1);
    const char* pins[] = {pinA, pinB};
    const char* params[] = {bufESR};
    showComponentResult(CAPACITOR_SYM, "Condensatore - ESR", pins, 2, params, 1);

    delay(500);
}

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

    // Calcola ESR e salva in variabile
    float esr = 0.0f;
    detectESR(a, b, &esr);

    // --- Nuova visualizzazione ---
    char bufC[24], bufESR[20];
    if (c < 1e-9)
        snprintf(bufC, sizeof(bufC), "C = %.2f pF", c * 1e12);
    else if (c < 1e-6)
        snprintf(bufC, sizeof(bufC), "C = %.2f nF", c * 1e9);
    else if (c < 1e-3)
        snprintf(bufC, sizeof(bufC), "C = %.2f uF", c * 1e6);
    else
        snprintf(bufC, sizeof(bufC), "C = %.2f mF", c * 1e3);

    if (esr < 1.0)
        snprintf(bufESR, sizeof(bufESR), "ESR = %.2f Ohm", esr);
    else
        snprintf(bufESR, sizeof(bufESR), "ESR = %.1f Ohm", esr);

    char pinA[8], pinB[8];
    snprintf(pinA, sizeof(pinA), "TP%d", a+1);
    snprintf(pinB, sizeof(pinB), "TP%d", b+1);
    const char* pins[] = {pinA, pinB};
    const char* params[] = {bufC, bufESR};
    showComponentResult(CAPACITOR_SYM, "Condensatore", pins, 2, params, 2);
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
            char bufType[16] = "NPN";
            char bufE[8], bufB[8], bufC[8];
            snprintf(bufE, sizeof(bufE), "E=TP%d", e+1);
            snprintf(bufB, sizeof(bufB), "B=TP%d", b+1);
            snprintf(bufC, sizeof(bufC), "C=TP%d", c+1);
            const char* pins[] = {bufE, bufB, bufC};
            const char* params[] = {bufType};
            showComponentResult(BJT_NPN_SYM, "BJT", pins, 3, params, 1);
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
            char bufType[16] = "PNP";
            char bufE[8], bufB[8], bufC[8];
            snprintf(bufE, sizeof(bufE), "E=TP%d", e+1);
            snprintf(bufB, sizeof(bufB), "B=TP%d", b+1);
            snprintf(bufC, sizeof(bufC), "C=TP%d", c+1);
            const char* pins[] = {bufE, bufB, bufC};
            const char* params[] = {bufType};
            showComponentResult(BJT_PNP_SYM, "BJT", pins, 3, params, 1);
            return;
        }
    }
    // Nessun BJT trovato
    const char* pins[] = {};
    showComponentResult(BJT_NPN_SYM, "Nessun BJT trovato", pins, 0, nullptr, 0);
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

    char pinG[16], pinS[16], pinD[16];
    snprintf(pinG, sizeof(pinG), "G=TP%d", 1);
    snprintf(pinS, sizeof(pinS), "S=TP%d", 2);
    snprintf(pinD, sizeof(pinD), "D=TP%d", 3);
    const char* pins[] = {pinG, pinS, pinD};

    if (v_ds < 0.1 && v_gs > 1.5) {
        const char* params[] = {"N-MOSFET"};
        showComponentResult(MOSFET_ENH_N_CH_SYM, "MOSFET", pins, 3, params, 1);
        return;
    }
    showComponentResult(MOSFET_ENH_N_CH_SYM, "Nessun MOSFET trovato", pins, 3, nullptr, 0);
}

// --- 8. JFET ---
void detectJFET() {
    struct JFETTest { TP s, d, g; };
    const JFETTest combos[6] = {
        {TEST_POINT_1, TEST_POINT_2, TEST_POINT_3}, {TEST_POINT_1, TEST_POINT_3, TEST_POINT_2},
        {TEST_POINT_2, TEST_POINT_1, TEST_POINT_3}, {TEST_POINT_2, TEST_POINT_3, TEST_POINT_1},
        {TEST_POINT_3, TEST_POINT_1, TEST_POINT_2}, {TEST_POINT_3, TEST_POINT_2, TEST_POINT_1}
    };

    for (int i = 0; i < 6; ++i) {
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

        char pins_s[12], pins_d[12], pins_g[12];
        snprintf(pins_s, sizeof(pins_s), "S=TP%d", s+1);
        snprintf(pins_d, sizeof(pins_d), "D=TP%d", d+1);
        snprintf(pins_g, sizeof(pins_g), "G=TP%d", g+1);
        const char* pins[] = {pins_s, pins_d, pins_g};

        if (is_njfet) {
            const char* params[] = {"N-JFET"};
            showComponentResult(JFET_N_CH_SYM, "JFET", pins, 3, params, 1);
            return;
        } else if (is_pjfet) {
            const char* params[] = {"P-JFET"};
            showComponentResult(JFET_P_CH_SYM, "JFET", pins, 3, params, 1);
            return;
        }
    }
    // Nessun JFET trovato
    showComponentResult(JFET_N_CH_SYM, "Nessun JFET trovato", nullptr, 0, nullptr, 0);
}

// --- 9. IGBT ---
void detectIGBT() {
    struct IGBTTest { TP c, e, g; };
    const IGBTTest combos[6] = {
        {TEST_POINT_1, TEST_POINT_2, TEST_POINT_3}, {TEST_POINT_1, TEST_POINT_3, TEST_POINT_2},
        {TEST_POINT_2, TEST_POINT_1, TEST_POINT_3}, {TEST_POINT_2, TEST_POINT_3, TEST_POINT_1},
        {TEST_POINT_3, TEST_POINT_1, TEST_POINT_2}, {TEST_POINT_3, TEST_POINT_2, TEST_POINT_1}
    };

    for (int i = 0; i < 6; ++i) {
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

        char bufC[12], bufE[12], bufG[12];
        snprintf(bufC, sizeof(bufC), "C=TP%d", c+1);
        snprintf(bufE, sizeof(bufE), "E=TP%d", e+1);
        snprintf(bufG, sizeof(bufG), "G=TP%d", g+1);
        const char* pins[] = {bufC, bufE, bufG};

        if (v_ce_before > 2.0 && v_ce_after < 0.5 && v_ce_conduct < 0.5) {
            const char* params[] = {"IGBT"};
            showComponentResult(IGBT_ENH_N_CH_SYM, "IGBT", pins, 3, params, 1);
            return;
        }
    }

    // Nessun IGBT trovato
    showComponentResult(IGBT_ENH_N_CH_SYM, "Nessun IGBT trovato", nullptr, 0, nullptr, 0);
}

// --- 10. SCR ---
void detectSCR() {
    struct SCRTest { TP a, k, g; };
    const SCRTest combos[6] = {
        {TEST_POINT_1, TEST_POINT_2, TEST_POINT_3}, {TEST_POINT_1, TEST_POINT_3, TEST_POINT_2},
        {TEST_POINT_2, TEST_POINT_1, TEST_POINT_3}, {TEST_POINT_2, TEST_POINT_3, TEST_POINT_1},
        {TEST_POINT_3, TEST_POINT_1, TEST_POINT_2}, {TEST_POINT_3, TEST_POINT_2, TEST_POINT_1}
    };

    for (int i = 0; i < 6; ++i) {
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

        char bufA[12], bufK[12], bufG[12];
        snprintf(bufA, sizeof(bufA), "A=TP%d", a+1);
        snprintf(bufK, sizeof(bufK), "K=TP%d", k+1);
        snprintf(bufG, sizeof(bufG), "G=TP%d", g+1);
        const char* pins[] = {bufA, bufK, bufG};

        if (v_ak_off > 2.0 && v_ak_on < 0.5) {
            const char* params[] = {"SCR"};
            showComponentResult(SCR_SYM, "SCR", pins, 3, params, 1);
            return;
        }
    }

    // Nessun SCR trovato
    showComponentResult(SCR_SYM, "Nessun SCR trovato", nullptr, 0, nullptr, 0);
}

// --- 11. Triac ---
void detectTriac() {
    struct TriacTest { TP mt1, mt2, g; };
    const TriacTest combos[6] = {
        {TEST_POINT_1, TEST_POINT_2, TEST_POINT_3}, {TEST_POINT_1, TEST_POINT_3, TEST_POINT_2},
        {TEST_POINT_2, TEST_POINT_1, TEST_POINT_3}, {TEST_POINT_2, TEST_POINT_3, TEST_POINT_1},
        {TEST_POINT_3, TEST_POINT_1, TEST_POINT_2}, {TEST_POINT_3, TEST_POINT_2, TEST_POINT_1}
    };

    for (int i = 0; i < 6; ++i) {
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

        char bufMT1[16], bufMT2[16], bufG[16];
        snprintf(bufMT1, sizeof(bufMT1), "MT1=TP%d", mt1+1);
        snprintf(bufMT2, sizeof(bufMT2), "MT2=TP%d", mt2+1);
        snprintf(bufG, sizeof(bufG), "G=TP%d", g+1);
        const char* pins[] = {bufMT1, bufMT2, bufG};

        if (v_mt12_off > 2.0 && v_mt12_on < 0.5) {
            const char* params[] = {"Triac"};
            showComponentResult(TRIAC_SYM, "Triac", pins, 3, params, 1);
            return;
        }
    }

    // Nessun Triac trovato
    showComponentResult(TRIAC_SYM, "Nessun Triac trovato", nullptr, 0, nullptr, 0);
}

// --- 12. PUT ---
void detectPUT() {
    struct PUTTest { TP a, k, g, r; };
    const PUTTest combos[24] = {
        {TEST_POINT_1, TEST_POINT_2, TEST_POINT_3, TEST_POINT_1},
        {TEST_POINT_1, TEST_POINT_2, TEST_POINT_3, TEST_POINT_2},
        {TEST_POINT_1, TEST_POINT_2, TEST_POINT_3, TEST_POINT_3},
        {TEST_POINT_1, TEST_POINT_3, TEST_POINT_2, TEST_POINT_1},
        {TEST_POINT_1, TEST_POINT_3, TEST_POINT_2, TEST_POINT_2},
        {TEST_POINT_1, TEST_POINT_3, TEST_POINT_2, TEST_POINT_3},
        {TEST_POINT_2, TEST_POINT_1, TEST_POINT_3, TEST_POINT_1},
        {TEST_POINT_2, TEST_POINT_1, TEST_POINT_3, TEST_POINT_2},
        {TEST_POINT_2, TEST_POINT_1, TEST_POINT_3, TEST_POINT_3},
        {TEST_POINT_2, TEST_POINT_3, TEST_POINT_1, TEST_POINT_1},
        {TEST_POINT_2, TEST_POINT_3, TEST_POINT_1, TEST_POINT_2},
        {TEST_POINT_2, TEST_POINT_3, TEST_POINT_1, TEST_POINT_3},
        {TEST_POINT_3, TEST_POINT_1, TEST_POINT_2, TEST_POINT_1},
        {TEST_POINT_3, TEST_POINT_1, TEST_POINT_2, TEST_POINT_2},
        {TEST_POINT_3, TEST_POINT_1, TEST_POINT_2, TEST_POINT_3},
        {TEST_POINT_3, TEST_POINT_2, TEST_POINT_1, TEST_POINT_1},
        {TEST_POINT_3, TEST_POINT_2, TEST_POINT_1, TEST_POINT_2},
        {TEST_POINT_3, TEST_POINT_2, TEST_POINT_1, TEST_POINT_3}
    };

    for (int i = 0; i < 18; ++i) {
        TP a = combos[i].a, k = combos[i].k, g = combos[i].g, r = combos[i].r;

        tp::floatAll();
        delay(10);

        // Step 1: Porta il catodo a LOW, anodo a HIGH, gate floating
        tp::setMode(a, OUTPUT); tp::write(a, HIGH);
        tp::setMode(k, OUTPUT); tp::write(k, LOW);
        tp::setMode(g, INPUT);
        delay(10);
        float v_ak_off = adc::readVoltage(a) - adc::readVoltage(k);

        // Step 2: Applica impulso al gate
        tp::setMode(g, OUTPUT); tp::write(g, HIGH);
        delay(5);
        tp::write(g, LOW);
        tp::setMode(g, INPUT);
        delay(5);
        float v_ak_on = adc::readVoltage(a) - adc::readVoltage(k);

        char bufA[12], bufK[12], bufG[12];
        snprintf(bufA, sizeof(bufA), "A=TP%d", a+1);
        snprintf(bufK, sizeof(bufK), "K=TP%d", k+1);
        snprintf(bufG, sizeof(bufG), "G=TP%d", g+1);
        const char* pins[] = {bufA, bufK, bufG};

        // Tipica caratteristica: v_ak_off alto, v_ak_on basso dopo impulso su gate
        if (v_ak_off > 2.0 && v_ak_on < 0.5) {
            const char* params[] = {"PUT"};
            showComponentResult(PUT_SYM, "PUT", pins, 3, params, 1);
            return;
        }
    }

    // Nessun PUT trovato
    showComponentResult(PUT_SYM, "Nessun PUT trovato", nullptr, 0, nullptr, 0);
}
  
// --- 13. UJT ---
void detectUJT() {
    struct UJTTest { TP e, b1, b2; };
    const UJTTest combos[6] = {
        {TEST_POINT_1, TEST_POINT_2, TEST_POINT_3}, {TEST_POINT_1, TEST_POINT_3, TEST_POINT_2},
        {TEST_POINT_2, TEST_POINT_1, TEST_POINT_3}, {TEST_POINT_2, TEST_POINT_3, TEST_POINT_1},
        {TEST_POINT_3, TEST_POINT_1, TEST_POINT_2}, {TEST_POINT_3, TEST_POINT_2, TEST_POINT_1}
    };

    for (int i = 0; i < 6; ++i) {
        TP e = combos[i].e, b1 = combos[i].b1, b2 = combos[i].b2;

        tp::floatAll();
        delay(10);

        // Step 1: Misura la resistenza tra B1 e B2 (generalmente bassa, qualche kOhm)
        tp::setMode(b1, OUTPUT); tp::write(b1, LOW);
        tp::setMode(b2, OUTPUT); tp::write(b2, HIGH);
        tp::setMode(e, INPUT);
        delay(10);
        float vb1 = adc::readVoltage(b1);
        float vb2 = adc::readVoltage(b2);
        float v_b1b2 = vb2 - vb1;
        float r_b1b2 = v_b1b2 / ((ADC_VREF - vb2) / TP_SERIES_RESISTANCE);

        // Step 2: Misura caduta tra E e B1 con E HIGH, B1 LOW, B2 floating
        tp::floatAll();
        tp::setMode(e, OUTPUT); tp::write(e, HIGH);
        tp::setMode(b1, OUTPUT); tp::write(b1, LOW);
        tp::setMode(b2, INPUT);
        delay(10);
        float ve = adc::readVoltage(e);
        float vb1_2 = adc::readVoltage(b1);
        float v_eb1 = ve - vb1_2;

        // Criteri tipici: R_B1B2 bassa (1k-10k), V_EB1 > 0.6V (polarizzazione giunzione)
        if (r_b1b2 > 500 && r_b1b2 < 20000 && v_eb1 > 0.5 && v_eb1 < 2.5) {
            char bufE[12], bufB1[12], bufB2[12], bufR[32], bufVE[32];
            snprintf(bufE, sizeof(bufE), "E=TP%d", e+1);
            snprintf(bufB1, sizeof(bufB1), "B1=TP%d", b1+1);
            snprintf(bufB2, sizeof(bufB2), "B2=TP%d", b2+1);

            if (r_b1b2 < 1000)
                snprintf(bufR, sizeof(bufR), "RB1B2 = %.1f Ω", r_b1b2);
            else if (r_b1b2 < 1000000)
                snprintf(bufR, sizeof(bufR), "RB1B2 = %.2f kΩ", r_b1b2 / 1000.0);
            else
                snprintf(bufR, sizeof(bufR), "RB1B2 = %.2f MΩ", r_b1b2 / 1000000.0);

            snprintf(bufVE, sizeof(bufVE), "Veb1 = %.2f V", v_eb1);

            const char* pins[] = {bufE, bufB1, bufB2};
            const char* params[] = {bufR, bufVE};
            showComponentResult(UJT_N_TYPE_SYM, "UJT", pins, 3, params, 2);
            return;
        }
    }

    // Nessun UJT trovato
    showComponentResult(UJT_N_TYPE_SYM, "Nessun UJT trovato", nullptr, 0, nullptr, 0);
}

// --- 14. Quarzo (Quartz Crystal) ---
void detectQuartzCrystal() {
    struct CrystalTest { TP a, b; };
    const CrystalTest combos[3] = {
        {TEST_POINT_1, TEST_POINT_2},
        {TEST_POINT_1, TEST_POINT_3},
        {TEST_POINT_2, TEST_POINT_3}
    };

    for (int i = 0; i < 3; ++i) {
        TP a = combos[i].a, b = combos[i].b;

        tp::floatAll();
        delay(10);

        // Misurazione della resistenza tra i due pin
        tp::setMode(a, OUTPUT); tp::write(a, HIGH);
        tp::setMode(b, OUTPUT); tp::write(b, LOW);
        delay(5);

        float va = adc::readVoltage(a);
        float vb = adc::readVoltage(b);
        float v_drop = va - vb;

        float current = (ADC_VREF - va) / TP_SERIES_RESISTANCE;
        if (current < 1e-7)
            current = 1e-7;
        float r = v_drop / current - TP_SERIES_RESISTANCE;

        // Un quarzo ha una resistenza molto alta, superiore anche a 1MΩ
        if (r > 1000000) {
            char bufA[12], bufB[12], bufR[32];
            snprintf(bufA, sizeof(bufA), "TP%d", a+1);
            snprintf(bufB, sizeof(bufB), "TP%d", b+1);
            snprintf(bufR, sizeof(bufR), "R = %.2f MΩ", r / 1000000.0);

            const char* pins[] = {bufA, bufB};
            const char* params[] = {bufR};
            showComponentResult(QUARTZ_CRYSTAL_SYM, "Quarzo", pins, 2, params, 1);
            return;
        }
    }

    // Nessun quarzo trovato
    showComponentResult(QUARTZ_CRYSTAL_SYM, "Nessun quarzo trovato", nullptr, 0, nullptr, 0);
}

// --- 15. OneWire ---
void detectOneWireDevice() {
    struct OneWireTest { TP data, vcc, gnd; };
    const OneWireTest combos[6] = {
        {TEST_POINT_1, TEST_POINT_2, TEST_POINT_3}, {TEST_POINT_1, TEST_POINT_3, TEST_POINT_2},
        {TEST_POINT_2, TEST_POINT_1, TEST_POINT_3}, {TEST_POINT_2, TEST_POINT_3, TEST_POINT_1},
        {TEST_POINT_3, TEST_POINT_1, TEST_POINT_2}, {TEST_POINT_3, TEST_POINT_2, TEST_POINT_1}
    };

    for (int i = 0; i < 6; ++i) {
        TP data = combos[i].data, vcc = combos[i].vcc, gnd = combos[i].gnd;

        tp::floatAll();
        delay(10);

        // Step 1: Pull-up su data (come da protocollo 1-Wire)
        tp::setMode(vcc, OUTPUT); tp::write(vcc, HIGH);
        tp::setMode(gnd, OUTPUT); tp::write(gnd, LOW);
        tp::setMode(data, INPUT_PULLUP); // Se disponibile, altrimenti OUTPUT HIGH e poi INPUT
        delay(10);

        float v_data = adc::readVoltage(data);

        // Step 2: Simula un reset pulse: porta data a LOW, poi rilascialo e misura presence pulse
        tp::setMode(data, OUTPUT); tp::write(data, LOW);
        delay(1);
        tp::setMode(data, INPUT); // Rilascia linea
        delay(1);
        float v_data_after = adc::readVoltage(data);

        // Criterio: sulla linea 1-Wire, la presenza di un dispositivo genera un "presence pulse" (caduta breve a LOW)
        if (v_data > 2.0 && v_data_after < 1.0) {
            char bufD[12], bufVCC[12], bufGND[12];
            snprintf(bufD, sizeof(bufD), "DATA=TP%d", data+1);
            snprintf(bufVCC, sizeof(bufVCC), "VCC=TP%d", vcc+1);
            snprintf(bufGND, sizeof(bufGND), "GND=TP%d", gnd+1);

            const char* pins[] = {bufD, bufVCC, bufGND};
            const char* params[] = {"Presence pulse rilevata"};
            showComponentResult(ONEWIRE_DEVICE_SYM, "One Wire", pins, 3, params, 1);
            return;
        }
    }

    // Nessun device One Wire trovato
    showComponentResult(ONEWIRE_DEVICE_SYM, "Nessun One Wire trovato", nullptr, 0, nullptr, 0);
}

// --- 16. Zener (con step-up) ---


void detectZenerStepUp() {
    struct ZenerTest { TP a, k; };
    const ZenerTest combos[6] = {
        {TEST_POINT_1, TEST_POINT_2}, {TEST_POINT_2, TEST_POINT_1},
        {TEST_POINT_1, TEST_POINT_3}, {TEST_POINT_3, TEST_POINT_1},
        {TEST_POINT_2, TEST_POINT_3}, {TEST_POINT_3, TEST_POINT_2}
    };

    for (int i = 0; i < 6; ++i) {
        TP a = combos[i].a, k = combos[i].k;

        tp::floatAll();
        delay(10);

        // Step 1: Test Zener in modo normale (diretto e inverso)
        tp::setMode(a, OUTPUT); tp::write(a, HIGH);
        tp::setMode(k, OUTPUT); tp::write(k, LOW);
        delay(10);

        float va = adc::readVoltage(a);
        float vk = adc::readVoltage(k);
        float vf = va - vk;

        // Test in inversa con Vcc normale
        tp::floatAll();
        tp::setMode(a, OUTPUT); tp::write(a, LOW);
        tp::setMode(k, OUTPUT); tp::write(k, HIGH);
        delay(10);

        va = adc::readVoltage(a);
        vk = adc::readVoltage(k);
        float vr = vk - va;

        // Se la tensione inversa non supera la soglia, abilita step-up
        bool detected = false;
        float vz_stepup = 0.0;

        if (vr < 3.0) { // Zener non conduttivo con Vcc normale, prova con step-up
            pinMode(GPIO_STEPUP_EN, OUTPUT);
            digitalWrite(GPIO_STEPUP_EN, HIGH);  // attiva boost converter
            delay(20);
            tp::floatAll();
            tp::setMode(a, OUTPUT); tp::write(a, LOW);
            tp::setMode(k, OUTPUT); tp::write(k, HIGH);
            delay(20);

            va = adc::readVoltage(a);
            vk = adc::readVoltage(k);
            vz_stepup = vk - va;

            digitalWrite(GPIO_STEPUP_EN, LOW); // disattiva boost converter

            if (vz_stepup > 2.5 && vz_stepup < 30.0) { // tipico range zener
                detected = true;
            }
        } else if (vr > 2.5 && vr < 10.0) { // già trovato, tipico range zener
            vz_stepup = vr;
            detected = true;
        }

        char bufA[12], bufK[12], bufV[24];
        snprintf(bufA, sizeof(bufA), "A=TP%d", a+1);
        snprintf(bufK, sizeof(bufK), "K=TP%d", k+1);
        snprintf(bufV, sizeof(bufV), "Vz = %.2f V", vz_stepup);

        const char* pins[] = {bufA, bufK};
        const char* params[] = {bufV};

        if (detected) {
            showComponentResult(ZENER_DIODE_SYM, "Zener", pins, 2, params, 1);
            return;
        }
    }

    // Nessuna zener trovata
    showComponentResult(ZENER_DIODE_SYM, "Nessuna zener trovata", nullptr, 0, nullptr, 0);
}



// --- 17. LED (con step-up) ---
void detectLedStepUp() {
    struct LedTest { TP a, k; };
    const LedTest combos[6] = {
        {TEST_POINT_1, TEST_POINT_2}, {TEST_POINT_2, TEST_POINT_1},
        {TEST_POINT_1, TEST_POINT_3}, {TEST_POINT_3, TEST_POINT_1},
        {TEST_POINT_2, TEST_POINT_3}, {TEST_POINT_3, TEST_POINT_2}
    };

    for (int i = 0; i < 6; ++i) {
        TP a = combos[i].a, k = combos[i].k;

        tp::floatAll();
        delay(10);

        // Test diretto con Vcc standard
        tp::setMode(a, OUTPUT); tp::write(a, HIGH);
        tp::setMode(k, OUTPUT); tp::write(k, LOW);
        delay(10);

        float va = adc::readVoltage(a);
        float vk = adc::readVoltage(k);
        float vf = va - vk; // tensione diretta LED

        bool detected = false;
        float vf_stepup = 0.0;

        // Se tensione diretta troppo bassa, prova con step-up
        if (vf < 1.7) { // LED non acceso o è un LED ad alta soglia
           pinMode(GPIO_STEPUP_EN, OUTPUT);
            digitalWrite(GPIO_STEPUP_EN, HIGH);  // attiva boost converter
            delay(20);
            tp::floatAll();
            tp::setMode(a, OUTPUT); tp::write(a, HIGH);
            tp::setMode(k, OUTPUT); tp::write(k, LOW);
            delay(20);

            va = adc::readVoltage(a);
            vk = adc::readVoltage(k);
            vf_stepup = va - vk;

            digitalWrite(GPIO_STEPUP_EN, LOW);; // disattiva boost converter

            if (vf_stepup > 1.7 && vf_stepup < 10.0) { // tipico range LED
                detected = true;
            }
        } else if (vf > 1.7 && vf < 5.0) { // LED trovato già con Vcc normale
            vf_stepup = vf;
            detected = true;
        }

        char bufA[12], bufK[12], bufV[24];
        snprintf(bufA, sizeof(bufA), "A=TP%d", a+1);
        snprintf(bufK, sizeof(bufK), "K=TP%d", k+1);
        snprintf(bufV, sizeof(bufV), "Vf = %.2f V", vf_stepup);

        const char* pins[] = {bufA, bufK};
        const char* params[] = {bufV};

        if (detected) {
            showComponentResult(LED_SYM, "LED", pins, 2, params, 1);
            return;
        }
    }

    // Nessun LED trovato
    showComponentResult(LED_SYM, "Nessun LED trovato", nullptr, 0, nullptr, 0);
}

// Nuova funzione per testare un doppio diodo a tre piedini
void detectDoubleDiodeOrLED(TP p1, TP p2, TP p3) {
    // Prova tutte le configurazioni possibili di pin
    struct Combo { TP common, a1, a2; };
    const Combo combos[3] = {
        {p1, p2, p3},
        {p2, p1, p3},
        {p3, p1, p2}
    };

    for (int i = 0; i < 3; ++i) {
        TP common = combos[i].common, p_a1 = combos[i].a1, p_a2 = combos[i].a2;

        tp::floatAll();
        delay(5);

        // Test 1: Common come catodo (catodo comune)
        // Polarizza anodo1→comune, misura Vf1
        tp::setMode(p_a1, OUTPUT); tp::write(p_a1, HIGH);
        tp::setMode(common, OUTPUT); tp::write(common, LOW);
        tp::setMode(p_a2, INPUT);
        delay(5);
        float v1 = adc::readVoltage(p_a1) - adc::readVoltage(common);

        // Polarizza anodo2→comune, misura Vf2
        tp::setMode(p_a1, INPUT);
        tp::setMode(p_a2, OUTPUT); tp::write(p_a2, HIGH);
        tp::setMode(common, OUTPUT); tp::write(common, LOW);
        delay(5);
        float v2 = adc::readVoltage(p_a2) - adc::readVoltage(common);

        bool isDiode1 = v1 > 0.15 && v1 < 1.2; // tipico diodo/LED forward
        bool isDiode2 = v2 > 0.15 && v2 < 1.2;

        bool isLED1 = v1 >= 1.2 && v1 < 4.0; // tipico LED
        bool isLED2 = v2 >= 1.2 && v2 < 4.0;

        if ((isDiode1 || isLED1) && (isDiode2 || isLED2)) {
            // Identificato doppio diodo/LED a catodo comune
            char bufCommon[16], bufA1[16], bufA2[16];
            snprintf(bufCommon, sizeof(bufCommon), "K=TP%d", common+1);
            snprintf(bufA1, sizeof(bufA1), "A1=TP%d", p_a1+1);
            snprintf(bufA2, sizeof(bufA2), "A2=TP%d", p_a2+1);

            char bufV1[24], bufV2[24];
            snprintf(bufV1, sizeof(bufV1), "Vf1=%.2fV", v1);
            snprintf(bufV2, sizeof(bufV2), "Vf2=%.2fV", v2);

            const char* pins[] = {bufA1, bufA2, bufCommon};
            const char* params[] = {bufV1, bufV2};

            const char* name = (isLED1 || isLED2) ? "Doppio LED Catodo Comune" : "Doppio Diodo Catodo Comune";
            showComponentResult((isLED1 || isLED2) ? LED_SYM : DOUBLE_DIODE_SYM, name, pins, 3, params, 2);
            return;
        }

        // Test 2: Common come anodo (anodo comune)
        tp::floatAll();
        delay(5);

        // Polarizza comune→catodo1, misura Vf1
        tp::setMode(common, OUTPUT); tp::write(common, HIGH);
        tp::setMode(p_a1, OUTPUT); tp::write(p_a1, LOW);
        tp::setMode(p_a2, INPUT);
        delay(5);
        v1 = adc::readVoltage(common) - adc::readVoltage(p_a1);

        // Polarizza comune→catodo2, misura Vf2
        tp::setMode(p_a1, INPUT);
        tp::setMode(p_a2, OUTPUT); tp::write(p_a2, LOW);
        tp::setMode(common, OUTPUT); tp::write(common, HIGH);
        delay(5);
        v2 = adc::readVoltage(common) - adc::readVoltage(p_a2);

        isDiode1 = v1 > 0.15 && v1 < 1.2;
        isDiode2 = v2 > 0.15 && v2 < 1.2;
        isLED1 = v1 >= 1.2 && v1 < 4.0;
        isLED2 = v2 >= 1.2 && v2 < 4.0;

        if ((isDiode1 || isLED1) && (isDiode2 || isLED2)) {
            // Identificato doppio diodo/LED a anodo comune
            char bufCommon[16], bufK1[16], bufK2[16];
            snprintf(bufCommon, sizeof(bufCommon), "A=TP%d", common+1);
            snprintf(bufK1, sizeof(bufK1), "K1=TP%d", p_a1+1);
            snprintf(bufK2, sizeof(bufK2), "K2=TP%d", p_a2+1);

            char bufV1[24], bufV2[24];
            snprintf(bufV1, sizeof(bufV1), "Vf1=%.2fV", v1);
            snprintf(bufV2, sizeof(bufV2), "Vf2=%.2fV", v2);

            const char* pins[] = {bufCommon, bufK1, bufK2};
            const char* params[] = {bufV1, bufV2};

            const char* name = (isLED1 || isLED2) ? "Doppio LED Anodo Comune" : "Doppio Diodo Anodo Comune";
            showComponentResult((isLED1 || isLED2) ? LED_SYM : DOUBLE_DIODE_SYM, name, pins, 3, params, 2);
            return;
        }
    }

    // Nessun doppio diodo/LED trovato
    showComponentResult(DOUBLE_DIODE_SYM, "Doppio diodo/LED non rilevato", nullptr, 0, nullptr, 0);
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

void showComponentResult(
    int symbolIndex,
    const char* componentName,
    const char* pinLabels[],
    int numPins,
    const char* paramLabels[],
    int numParams
) {
    using namespace display;
    clear();

    // Icona centrata in alto (stile welcome)
    int scale = 2;
    int iconW = SYMBOL_SIZE_X * scale;
    int iconH = SYMBOL_SIZE_Y * scale;
    int centerX = screenWidth() / 2;
    int iconX = centerX - (iconW / 2);
    int iconY = 10;
    showIcon(symbolIndex, iconX, iconY, TFT_WHITE, scale);

    // Nome componente centrato
    setTextSize(2);
    int titleY = iconY + iconH + 8;
    int titleW = textWidth(componentName);
    setCursor(centerX - titleW / 2, titleY);
    setTextColor(TFT_YELLOW, TFT_BLACK);
    print(componentName);

    // Pinout (sotto il nome)
    int pinY = titleY + fontHeight() + 8;
    setTextColor(TFT_CYAN, TFT_BLACK);
    setTextSize(1);
    for (int i = 0; i < numPins; ++i) {
        setCursor(20, pinY + i * (fontHeight() + 2));
        print(pinLabels[i]);
    }

    // Parametri (sotto i pin)
    int paramY = pinY + numPins * (fontHeight() + 6);
    setTextColor(TFT_WHITE, TFT_BLACK);
    for (int i = 0; i < numParams; ++i) {
        setCursor(20, paramY + i * (fontHeight() + 2));
        print(paramLabels[i]);
    }
}

  
} // namespace test