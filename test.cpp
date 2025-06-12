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
        snprintf(buf, sizeof(buf), "%.2f kOhm", r/1000.0f);
        display::showMessage(buf);
    } else {
        char buf[16];
        snprintf(buf, sizeof(buf), "%.2f MOhm", r/1e6f);
        display::showMessage(buf);
    }
}
void showInd(float l) {
    if (l < 1e-6)
        display::showMessage("Induttanza troppo bassa");
    else if (l < 1e-3) {
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
    tp::setMode(a, OUTPUT); tp::write(a, HIGH);
    tp::setMode(b, OUTPUT); tp::write(b, LOW);

    delay(5);
    float va = adc::readVoltage(a);
    float vb = adc::readVoltage(b);
    float v_drop = va - vb;
    float current = (ADC_VREF - va) / TP_SERIES_RESISTANCE;
    if (current < 1e-7) current = 1e-7;
    float r = v_drop / current - TP_SERIES_RESISTANCE;

    display::clear();
    display::showMessage("Resistenza tra TP:");
    showRes(r);
}
void detectResistors() {
    detectResistorTP(TP1, TP2); delay(500);
    detectResistorTP(TP2, TP3); delay(500);
    detectResistorTP(TP3, TP1); delay(500);
}

// --- 2. Diodi/LED ---
void detectDiodeTP(TP a, TP b) {
    tp::floatAll();
    delay(5);
    tp::setMode(a, OUTPUT); tp::write(a, HIGH);
    tp::setMode(b, OUTPUT); tp::write(b, LOW);
    delay(5);
    float v_fwd = adc::readVoltage(a) - adc::readVoltage(b);
    tp::write(a, LOW); tp::write(b, HIGH);
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
    detectDiodeTP(TP1, TP2); delay(500);
    detectDiodeTP(TP2, TP3); delay(500);
    detectDiodeTP(TP3, TP1); delay(500);
}

// --- 3. Zener ---
void detectZenerTP(TP a, TP b) {
    // Principio: polarizzo il diodo inversamente e misuro la tensione di soglia
    tp::floatAll(); delay(5);
    tp::setMode(a, OUTPUT); tp::write(a, LOW);
    tp::setMode(b, OUTPUT); tp::write(b, HIGH);
    delay(10);
    float v_zener = adc::readVoltage(a) - adc::readVoltage(b);

    display::clear();
    if (v_zener > 2.0f && v_zener < 24.0f) { // tipico range Zener
        display::showMessage("Zener rilevato:");
        display::showVoltage(v_zener, "Vz");
    } else {
        display::showMessage("Nessun Zener");
    }
}
void detectZenerDiodes() {
    detectZenerTP(TP1, TP2); delay(500);
    detectZenerTP(TP2, TP3); delay(500);
    detectZenerTP(TP3, TP1); delay(500);
}

// --- 4. Induttori ---
void detectInductorTP(TP a, TP b) {
    // Metodo semplificato GM328: applico un impulso noto, misuro la variazione di corrente/tensione
    // Nota: molto semplificato, la precisione dipende dall'implementazione hardware!
    tp::floatAll();
    delay(5);

    // Scarica res. parassite
    tp::setMode(a, OUTPUT); tp::write(a, LOW);
    tp::setMode(b, OUTPUT); tp::write(b, LOW);
    delay(10);

    // Impulso: a HIGH, b INPUT
    tp::setMode(a, OUTPUT); tp::write(a, HIGH);
    tp::setMode(b, INPUT);
    delayMicroseconds(100);

    // Misura picco su b
    float v_peak = adc::readVoltage(b);

    // Formula (da GM328): L = (Vref * dt) / dI
    // Qui dt = durata impulso, dI stimata in base a R serie
    float dt = 100e-6f; // 100 us
    float dI = ADC_VREF / TP_SERIES_RESISTANCE;
    float L = (v_peak * dt) / dI;

    display::clear();
    display::showMessage("Induttanza TP:");
    showInd(L);
}
void detectInductors() {
    detectInductorTP(TP1, TP2); delay(1000);
    detectInductorTP(TP2, TP3); delay(1000);
    detectInductorTP(TP3, TP1); delay(1000);
}

// --- 5. Condensatori ---
void detectCapacitorTP(TP a, TP b) {
    tp::floatAll(); delay(5);
    tp::setMode(a, OUTPUT); tp::write(a, LOW);
    tp::setMode(b, OUTPUT); tp::write(b, LOW);
    delay(50);

    tp::setMode(a, OUTPUT); tp::write(a, HIGH);
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
        snprintf(buf, sizeof(buf), "%.2f pF", c*1e12);
    else if (c < 1e-6)
        snprintf(buf, sizeof(buf), "%.2f nF", c*1e9);
    else if (c < 1e-3)
        snprintf(buf, sizeof(buf), "%.2f uF", c*1e6);
    else
        snprintf(buf, sizeof(buf), "%.2f mF", c*1e3);
    display::showMessage(buf);
}
void detectCapacitors() {
    detectCapacitorTP(TP1, TP2); delay(1000);
    detectCapacitorTP(TP2, TP3); delay(1000);
    detectCapacitorTP(TP3, TP1); delay(1000);
}

// --- 6. BJT ---
void detectBJT() {
    struct BJTTest { TP e, b, c; } combos[6] = {
        {TP1, TP2, TP3}, {TP1, TP3, TP2},
        {TP2, TP1, TP3}, {TP2, TP3, TP1},
        {TP3, TP1, TP2}, {TP3, TP2, TP1}
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
    // Esempio N-MOSFET (solo logica base)
    tp::floatAll();
    tp::setMode(TP1, OUTPUT); tp::write(TP1, HIGH);
    tp::setMode(TP2, OUTPUT); tp::write(TP2, LOW);
    tp::setMode(TP3, INPUT);
    delay(5);
    float v_ds = adc::readVoltage(TP3) - adc::readVoltage(TP2);
    float v_gs = adc::readVoltage(TP1) - adc::readVoltage(TP2);
    if (v_ds < 0.1 && v_gs > 1.5) {
        display::clear();
        display::showMessage("N-MOSFET rilevato");
        display::showMessage("G=TP1 S=TP2 D=TP3");
        return;
    }
    display::clear();
    display::showMessage("Nessun MOSFET trovato");
}

// --- 8. JFET ---
// Rileva e identifica JFET N/P, ispirato alla logica GM328/HoRo
void test::detectJFET() {
    display::clear();
    display::showMessage("Rilevamento JFET...");

    // Combinazioni possibili di TP: (S, D, G)
    struct JFETTest { TP s, d, g; };
    const JFETTest combos[6] = {
        {TP1, TP2, TP3}, {TP1, TP3, TP2},
        {TP2, TP1, TP3}, {TP2, TP3, TP1},
        {TP3, TP1, TP2}, {TP3, TP2, TP1}
    };

    bool found = false;
    for (int i = 0; i < 6 && !found; ++i) {
        TP s = combos[i].s, d = combos[i].d, g = combos[i].g;

        // 1. Misura la conduzione S-D con G floating (INPUT)
        tp::floatAll();
        tp::setMode(g, INPUT);  // gate flottante
        tp::setMode(s, OUTPUT); tp::write(s, LOW);
        tp::setMode(d, OUTPUT); tp::write(d, HIGH);
        delay(10);

        float vs_f = adc::readVoltage(s);
        float vd_f = adc::readVoltage(d);
        float v_sd_f = vd_f - vs_f;

        // 2. Misura la conduzione S-D con Gate polarizzato (LOW per N, HIGH per P)
        // 2a. Gate a LOW (N-JFET test)
        tp::setMode(g, OUTPUT); tp::write(g, LOW);  // gate a GND
        delay(10);
        float vs_n = adc::readVoltage(s);
        float vd_n = adc::readVoltage(d);
        float v_sd_n = vd_n - vs_n;

        // 2b. Gate a HIGH (P-JFET test)
        tp::write(g, HIGH);
        delay(10);
        float vs_p = adc::readVoltage(s);
        float vd_p = adc::readVoltage(d);
        float v_sd_p = vd_p - vs_p;

        // 3. Analisi risultati:
        // Se G floating: v_sd_f bassa (canale conduttivo)
        // Se Gate LOW (N-JFET): v_sd_n molto più alta (canale "chiuso")
        // Se Gate HIGH (P-JFET): v_sd_p molto più alta (canale "chiuso")

        // Soglie empiriche (adatta al tuo hardware, qui stile HoRo):
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
        // Al prossimo giro cambia combinazione
    }
    if (!found) {
        display::clear();
        display::showMessage("Nessun JFET trovato");
    }
}

// --- 9. IGBT ---
// Rileva e identifica IGBT, ispirato alla logica GM328/HoRo
void test::detectIGBT() {
    display::clear();
    display::showMessage("Rilevamento IGBT...");

    // Combinazioni possibili di TP: (C, E, G)
    struct IGBTTest { TP c, e, g; };
    const IGBTTest combos[6] = {
        {TP1, TP2, TP3}, {TP1, TP3, TP2},
        {TP2, TP1, TP3}, {TP2, TP3, TP1},
        {TP3, TP1, TP2}, {TP3, TP2, TP1}
    };

    bool found = false;
    for (int i = 0; i < 6 && !found; ++i) {
        TP c = combos[i].c, e = combos[i].e, g = combos[i].g;

        // 1. Prepara tutti i TP in alta impedenza
        tp::floatAll();
        delay(10);

        // 2. Applica impulso sul Gate
        tp::setMode(g, OUTPUT); tp::write(g, HIGH); // Gate HIGH
        tp::setMode(e, OUTPUT); tp::write(e, LOW);  // Emitter LOW
        tp::setMode(c, INPUT);                      // Collector INPUT
        delay(10);

        // 3. Riporta il Gate a floating
        tp::setMode(g, INPUT); // Gate floating

        // 4. Misura la tensione Collector-Emitter prima e dopo l'impulso
        float v_ce_before = adc::readVoltage(c) - adc::readVoltage(e);

        // 5. Applica breve impulso Gate
        tp::setMode(g, OUTPUT); tp::write(g, HIGH);
        delay(10);
        tp::setMode(g, INPUT); // Gate floating

        // 6. Misura la tensione Collector-Emitter dopo l'impulso
        float v_ce_after = adc::readVoltage(c) - adc::readVoltage(e);

        // 7. Applica LOW al Collector per vedere se conduce
        tp::setMode(c, OUTPUT); tp::write(c, LOW);
        tp::setMode(e, OUTPUT); tp::write(e, LOW);
        delay(10);
        tp::setMode(c, INPUT); // Collector INPUT

        // 8. Misura se la tensione su CE scende (conduzione)
        float v_ce_conduct = adc::readVoltage(c) - adc::readVoltage(e);

        // 9. Analisi logica (tolleranze da tarare a seconda del circuito)
        // Un IGBT condurrà tra C ed E DOPO un impulso su GATE (come MOSFET)
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
// Rileva e identifica SCR, ispirato alla logica GM328/HoRo
void test::detectSCR() {
    display::clear();
    display::showMessage("Rilevamento SCR...");

    // Combinazioni possibili di TP: (A, K, G)
    struct SCRTest { TP a, k, g; };
    const SCRTest combos[6] = {
        {TP1, TP2, TP3}, {TP1, TP3, TP2},
        {TP2, TP1, TP3}, {TP2, TP3, TP1},
        {TP3, TP1, TP2}, {TP3, TP2, TP1}
    };

    bool found = false;
    for (int i = 0; i < 6 && !found; ++i) {
        TP a = combos[i].a, k = combos[i].k, g = combos[i].g;

        // 1. Porta tutto in alta impedenza
        tp::floatAll();
        delay(10);

        // 2. Prepara: Anodo HIGH, Catodo LOW, Gate floating
        tp::setMode(a, OUTPUT); tp::write(a, HIGH);
        tp::setMode(k, OUTPUT); tp::write(k, LOW);
        tp::setMode(g, INPUT);
        delay(10);

        // 3. Verifica che tra A e K non vi sia conduzione (SCR spento)
        float v_ak_off = adc::readVoltage(a) - adc::readVoltage(k);

        // 4. Applica impulso su Gate (breve HIGH)
        tp::setMode(g, OUTPUT); tp::write(g, HIGH);
        delay(5);
        tp::write(g, LOW);
        tp::setMode(g, INPUT);
        delay(5);

        // 5. Verifica se ora c'è conduzione tra Anodo e Catodo (SCR acceso)
        float v_ak_on = adc::readVoltage(a) - adc::readVoltage(k);

        // 6. Analisi: se v_ak_off alto, v_ak_on basso (conduzione), è SCR
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
// Rileva e identifica Triac, ispirato alla logica GM328/HoRo
void test::detectTriac() {
    display::clear();
    display::showMessage("Rilevamento Triac...");

    // Combinazioni possibili di TP: (MT1, MT2, G)
    struct TriacTest { TP mt1, mt2, g; };
    const TriacTest combos[6] = {
        {TP1, TP2, TP3}, {TP1, TP3, TP2},
        {TP2, TP1, TP3}, {TP2, TP3, TP1},
        {TP3, TP1, TP2}, {TP3, TP2, TP1}
    };

    bool found = false;
    for (int i = 0; i < 6 && !found; ++i) {
        TP mt1 = combos[i].mt1, mt2 = combos[i].mt2, g = combos[i].g;

        // 1. Porta tutto in alta impedenza
        tp::floatAll();
        delay(10);

        // 2. Prepara: MT1 LOW, MT2 HIGH, Gate floating
        tp::setMode(mt1, OUTPUT); tp::write(mt1, LOW);
        tp::setMode(mt2, OUTPUT); tp::write(mt2, HIGH);
        tp::setMode(g, INPUT);
        delay(10);

        // 3. Verifica che tra MT1 e MT2 non vi sia conduzione (Triac spento)
        float v_mt12_off = adc::readVoltage(mt2) - adc::readVoltage(mt1);

        // 4. Applica impulso su Gate (breve HIGH)
        tp::setMode(g, OUTPUT); tp::write(g, HIGH);
        delay(5);
        tp::write(g, LOW);
        tp::setMode(g, INPUT);
        delay(5);

        // 5. Verifica se ora c'è conduzione tra MT1 e MT2 (Triac acceso)
        float v_mt12_on = adc::readVoltage(mt2) - adc::readVoltage(mt1);

        // 6. Analisi: se v_mt12_off alto, v_mt12_on basso (conduzione), è Triac
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

// --- 12. PUT (Programmable Unijunction Transistor) ---
// Rileva e identifica PUT, ispirato alla logica GM328/HoRo
void test::detectPUT() {
    display::clear();
    display::showMessage("Rilevamento PUT...");

    // Combinazioni possibili di TP: (A, K, G)
    struct PUTTest { TP a, k, g; };
    const PUTTest combos[6] = {
        {TP1, TP2, TP3}, {TP1, TP3, TP2},
        {TP2, TP1, TP3}, {TP2, TP3, TP1},
        {TP3, TP1, TP2}, {TP3, TP2, TP1}
    };

    bool found = false;
    for (int i = 0; i < 6 && !found; ++i) {
        TP a = combos[i].a, k = combos[i].k, g = combos[i].g;

        tp::floatAll();
        delay(10);

        // 1. Prepara: Anodo HIGH, Catodo LOW, Gate INPUT (floating)
        tp::setMode(a, OUTPUT); tp::write(a, HIGH);
        tp::setMode(k, OUTPUT); tp::write(k, LOW);
        tp::setMode(g, INPUT);
        delay(10);

        // 2. Misura la tensione A-K (dovrebbe risultare alta se spento)
        float v_ak_off = adc::readVoltage(a) - adc::readVoltage(k);

        // 3. Applica impulso su Gate (breve HIGH)
        tp::setMode(g, OUTPUT); tp::write(g, HIGH);
        delay(5);
        tp::write(g, LOW);
        tp::setMode(g, INPUT);
        delay(5);

        // 4. Misura la tensione A-K (dovrebbe scendere se il PUT si accende)
        float v_ak_on = adc::readVoltage(a) - adc::readVoltage(k);

        // 5. Analisi: se v_ak_off alto e v_ak_on molto più basso, probabile PUT
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

// --- 13. UJT (Unijunction Transistor) ---
// Rileva e identifica UJT, ispirato alla logica GM328/HoRo
void test::detectUJT() {
    display::clear();
    display::showMessage("Rilevamento UJT...");

    // Combinazioni possibili di TP: (B1, B2, E)
    struct UJTTest { TP b1, b2, e; };
    const UJTTest combos[6] = {
        {TP1, TP2, TP3}, {TP1, TP3, TP2},
        {TP2, TP1, TP3}, {TP2, TP3, TP1},
        {TP3, TP1, TP2}, {TP3, TP2, TP1}
    };

    bool found = false;
    for (int i = 0; i < 6 && !found; ++i) {
        TP b1 = combos[i].b1, b2 = combos[i].b2, e = combos[i].e;

        tp::floatAll();
        delay(10);

        // 1. Misura resistenza tra B1 e B2 (deve essere lineare e "media" - tipico UJT)
        tp::setMode(b1, OUTPUT); tp::write(b1, HIGH);
        tp::setMode(b2, OUTPUT); tp::write(b2, LOW);
        tp::setMode(e, INPUT);
        delay(10);
        float v_b1b2 = adc::readVoltage(b1) - adc::readVoltage(b2);

        // 2. Applica impulso su Emitter e misura variazione su B1/B2
        tp::setMode(e, OUTPUT); tp::write(e, HIGH);
        delay(5);
        float v_b1b2_after = adc::readVoltage(b1) - adc::readVoltage(b2);

        // 3. Analisi: la resistenza tra B1-B2 dovrebbe cambiare al trigger dell'emitter
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
// Rileva e identifica quarzi, stile GM328/HoRo
void test::detectQuartzCrystal() {
    display::clear();
    display::showMessage("Test quarzo...");

    // Nota: la rilevazione del quarzo richiede un oscillatore a bordo, tipicamente basato su inverter o analoghi.
    // Qui si può solo simulare la logica GM328/Horo: si tenta di generare un’oscillazione e di rilevarne la presenza.
    // L’implementazione reale dipende dal circuito HW!
    // In questa versione SW:
    // 1. Si collega il quarzo tra due TP (ad es. TP1 e TP2).
    // 2. Si genera un breve impulso alternato su TP1 e TP2 e si misura se su TP3 si rileva un segnale oscillante.

    // 1. Imposta TP1 e TP2 come OUTPUT, TP3 come INPUT analogico
    tp::floatAll();
    tp::setMode(TP1, OUTPUT);
    tp::setMode(TP2, OUTPUT);
    tp::setMode(TP3, INPUT);

    // 2. Simula oscillazione (impulsi rapidi alternati)
    for (int i = 0; i < 500; ++i) {
        tp::write(TP1, HIGH);
        tp::write(TP2, LOW);
        delayMicroseconds(2);
        tp::write(TP1, LOW);
        tp::write(TP2, HIGH);
        delayMicroseconds(2);
    }

    // 3. Leggi la tensione su TP3 per vedere se c'è una risposta significativa
    float v_tp3 = adc::readVoltage(TP3);

    // 4. Soglia empirica: se si rileva una variazione di tensione, probabile quarzo presente e oscillante
    if (v_tp3 > 0.2 && v_tp3 < (ADC_VREF - 0.2)) {
        display::showMessage("Quarzo rilevato!");
        // La frequenza reale non si può stimare senza HW dedicato
    } else {
        display::showMessage("Nessun quarzo trovato");
    }
}

// --- 15. OneWire ---
// Rileva presenza di dispositivi OneWire sulla linea, stile GM328/HoRo
void test::detectOneWireDevice() {
    display::clear();
    display::showMessage("Test OneWire...");

    // 1. Usa TP1 come linea 1-Wire, TP2 e TP3 floating
    tp::floatAll();
    tp::setMode(TP1, OUTPUT);
    tp::write(TP1, HIGH); // Pull-up
    delay(1);

    // 2. Invia reset pulse: TP1 LOW per 480us, poi HIGH
    tp::write(TP1, LOW);
    delayMicroseconds(480);
    tp::write(TP1, HIGH);
    // 3. Attendi 70us (presenza pulse parte)
    delayMicroseconds(70);

    // 4. Imposta TP1 come INPUT e misura la tensione (se scende, c'è un device)
    tp::setMode(TP1, INPUT);
    float v = adc::readVoltage(TP1);

    // 5. Attendi il termine del timeslot
    delayMicroseconds(410);

    // 6. Analisi: se la tensione scende dopo il reset pulse, c'è un dispositivo 1-Wire presente
    if (v < (ADC_VREF * 0.7)) {
        display::showMessage("OneWire trovato!");
    } else {
        display::showMessage("Nessun OneWire trovato");
    }
}

// --- 16. Zener ---

#define GPIO_STEPUP_EN  25 // Cambia con la tua effettiva GPIO di enable step-up


void test::detectZenerTP(TP a, TP b, int gpio_stepup) {
    // Abilita lo step-up per raggiungere tensioni fino a 35V
    pinMode(gpio_stepup, OUTPUT);
    digitalWrite(gpio_stepup, HIGH);
    delay(60); // Attendi stabilizzazione step-up

    tp::floatAll(); delay(5);
    tp::setMode(a, OUTPUT); tp::write(a, HIGH); // Anodo a Vstepup
    tp::setMode(b, OUTPUT); tp::write(b, LOW);  // Catodo a GND
    delay(30);

    float v_zener = adc::readVoltage(a) - adc::readVoltage(b);

    digitalWrite(gpio_stepup, LOW); // Disabilita lo step-up

    display::clear();
    if (v_zener > 2.0f && v_zener < 35.0f) {
        display::showMessage("Zener rilevato:");
        display::showVoltage(v_zener, "Vz");
    } else {
        display::showMessage("Nessun Zener");
    }
}

void test::detectZenerDiodes() {
    detectZenerTP(TP1, TP2, GPIO_STEPUP_EN); delay(500);
    detectZenerTP(TP2, TP3, GPIO_STEPUP_EN); delay(500);
    detectZenerTP(TP3, TP1, GPIO_STEPUP_EN); delay(500);
}


// --- 17. Led ---

void test::detectLEDTP(TP a, TP b, int gpio_stepup) {
    // Abilita step-up per LED ad alta Vf
    pinMode(gpio_stepup, OUTPUT);
    digitalWrite(gpio_stepup, HIGH);
    delay(50);

    tp::floatAll(); delay(5);
    tp::setMode(a, OUTPUT); tp::write(a, HIGH);
    tp::setMode(b, OUTPUT); tp::write(b, LOW);
    delay(15);

    float v_fwd = adc::readVoltage(a) - adc::readVoltage(b);

    digitalWrite(gpio_stepup, LOW); // Disabilita step-up

    display::clear();
    if (v_fwd > 1.3f && v_fwd < 4.5f) { // LED rosso ~1.6V, verde ~2V, bianco/UV anche >3V
        display::showMessage("LED rilevato:");
        display::showVoltage(v_fwd, "Vf");
        // Qui puoi aggiungere lettura fotodiodo/fototransistor per confermare l'emissione luminosa!
    } else {
        display::showMessage("Nessun LED");
    }
}

void test::detectLEDs() {
    detectLEDTP(TP1, TP2, GPIO_STEPUP_EN); delay(500);
    detectLEDTP(TP2, TP3, GPIO_STEPUP_EN); delay(500);
    detectLEDTP(TP3, TP1, GPIO_STEPUP_EN); delay(500);
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