#include "test.h"
#include "tp.h"
#include "adc.h"
#include "display.h"
#include "config.h"

#define STEPUP_EN  12  // GPIO che abilita lo step-up
#define VZ_MIN     2.5 // soglia minima per considerare un breakdown
#define VZ_MAX    35.0 // soglia massima misurabile

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

void test::detectDiodeTP1TP2() {
    float v_forward = 0.0f;
    float v_reverse = 0.0f;

    // Test direzione TP1 ➝ TP2
    tp::configureAsOutput(tp::TP1);
    tp::setLevel(tp::TP1, true);
    tp::configureAsInput(tp::TP2);
    delay(10);
    v_forward = adc::readVoltage(tp::TP2);

    // Test direzione TP2 ➝ TP1
    tp::configureAsOutput(tp::TP2);
    tp::setLevel(tp::TP2, true);
    tp::configureAsInput(tp::TP1);
    delay(10);
    v_reverse = adc::readVoltage(tp::TP1);

    display::clear();
    display::showMessage("Test Diodo TP1–TP2", TFT_CYAN);

    if (v_forward > 0.4f && v_forward < 0.85f && v_reverse < 0.15f) {
        display::showMessage("Diodo rilevato ➝", TFT_GREEN);
        display::tft.setCursor(10, 100);
        display::tft.printf("Anodo: TP1\nCatodo: TP2\nVf: %.2f V", v_forward);
    } else if (v_reverse > 0.4f && v_reverse < 0.85f && v_forward < 0.15f) {
        display::showMessage("Diodo rilevato ⇦", TFT_GREEN);
        display::tft.setCursor(10, 100);
        display::tft.printf("Anodo: TP2\nCatodo: TP1\nVf: %.2f V", v_reverse);
    } else {
        display::showMessage("Nessun diodo rilevato", TFT_RED);
        display::tft.setCursor(10, 100);
        display::tft.printf("Vf: %.2f V\nVr: %.2f V", v_forward, v_reverse);
    }
}

void test::detectDiodeBetween(tp::TPLabel a, tp::TPLabel b) {
    float v_ab = 0.0f;
    float v_ba = 0.0f;

    // Test direzione A ➝ B
    tp::configureAsOutput(a);
    tp::setLevel(a, true);
    tp::configureAsInput(b);
    delay(10);
    v_ab = adc::readVoltage(b);

    // Test direzione B ➝ A
    tp::configureAsOutput(b);
    tp::setLevel(b, true);
    tp::configureAsInput(a);
    delay(10);
    v_ba = adc::readVoltage(a);

    display::clear();
    display::showMessage("Test Diodo", TFT_CYAN);

    if (v_ab > 0.4f && v_ab < 0.85f && v_ba < 0.15f) {
        display::showMessage("Diodo rilevato ➝", TFT_GREEN);
        display::tft.setCursor(10, 100);
        display::tft.printf("Anodo: TP%d\nCatodo: TP%d\nVf: %.2f V", a + 1, b + 1, v_ab);
    } else if (v_ba > 0.4f && v_ba < 0.85f && v_ab < 0.15f) {
        display::showMessage("Diodo rilevato ⇦", TFT_GREEN);
        display::tft.setCursor(10, 100);
        display::tft.printf("Anodo: TP%d\nCatodo: TP%d\nVf: %.2f V", b + 1, a + 1, v_ba);
    } else {
        display::showMessage("Nessun diodo rilevato", TFT_RED);
        display::tft.setCursor(10, 100);
        display::tft.printf("TP%d–TP%d\nVf: %.2f V\nVr: %.2f V", a + 1, b + 1, v_ab, v_ba);
    }
}
void test::detectDoubleDiodeOrLED(tp::TPLabel a, tp::TPLabel b) {
    float v_ab = 0.0f;
    float v_ba = 0.0f;

    // Test A ➝ B
    tp::configureAsOutput(a);
    tp::setLevel(a, true);
    tp::configureAsInput(b);
    delay(10);
    v_ab = adc::readVoltage(b);

    // Test B ➝ A
    tp::configureAsOutput(b);
    tp::setLevel(b, true);
    tp::configureAsInput(a);
    delay(10);
    v_ba = adc::readVoltage(a);

    display::clear();
    display::showMessage("Test Diodo/LED", TFT_MAGENTA);

    const float LED_THRESHOLD_MIN = 1.5f;
    const float LED_THRESHOLD_MAX = 3.3f;

    bool ab_led = (v_ab >= LED_THRESHOLD_MIN && v_ab <= LED_THRESHOLD_MAX && v_ba < 0.2f);
    bool ba_led = (v_ba >= LED_THRESHOLD_MIN && v_ba <= LED_THRESHOLD_MAX && v_ab < 0.2f);

    bool both_diode =
        (v_ab > 0.4f && v_ab < 0.85f) &&
        (v_ba > 0.4f && v_ba < 0.85f);

    if (both_diode) {
        display::showMessage("Doppio diodo rilevato", TFT_GREEN);
        display::tft.setCursor(10, 100);
        display::tft.printf("TP%d ⇄ TP%d\nV1: %.2f V  V2: %.2f V", a + 1, b + 1, v_ab, v_ba);
    } else if (ab_led || ba_led) {
        display::showMessage("LED rilevato", TFT_YELLOW);
        display::tft.setCursor(10, 100);
        display::tft.printf("Anodo: TP%d\nCatodo: TP%d\nVf: %.2f V",
                            ab_led ? a + 1 : b + 1,
                            ab_led ? b + 1 : a + 1,
                            ab_led ? v_ab : v_ba);
    } else {
        display::showMessage("Nessun LED o doppio diodo", TFT_RED);
        display::tft.setCursor(10, 100);
        display::tft.printf("TP%d–TP%d\nV1: %.2f V  V2: %.2f V", a + 1, b + 1, v_ab, v_ba);
    }
}
void detectBJT() {
  using namespace tp;
  using namespace adc;
  using namespace display;

  int base = -1, t1 = -1, t2 = -1;
  bool isNPN = false, bjtFound = false;

  // Identifica la base tramite doppia giunzione
  for (int i = 0; i < 3; ++i) {
    int j = (i + 1) % 3;
    int k = (i + 2) % 3;

    float vij = detectDiodeBetween(i, j);
    float vik = detectDiodeBetween(i, k);
    float vji = detectDiodeBetween(j, i);
    float vki = detectDiodeBetween(k, i);

    if (vij > 0.4f && vik > 0.4f) {
      base = i; t1 = j; t2 = k;
      isNPN = true;
      bjtFound = true;
      break;
    } else if (vji > 0.4f && vki > 0.4f) {
      base = i; t1 = j; t2 = k;
      isNPN = false;
      bjtFound = true;
      break;
    }
  }

  if (!bjtFound) {
    showMessage("Nessun BJT rilevato");
    return;
  }

  // Polarizza la base
  setMode(base, Mode::OUT);
  write(base, isNPN ? HIGH : LOW);
  delay(5);

  // Imposta TP candidati a C/E come input
  setMode(t1, Mode::IN);
  setMode(t2, Mode::IN);
  setMode((isNPN ? t2 : t1), Mode::GND);  // Emettitore a GND

  delay(10);
  float vt1 = readVoltage(t1);
  float vt2 = readVoltage(t2);

  int collector, emitter;
  if ((isNPN && vt1 > vt2) || (!isNPN && vt1 < vt2)) {
    collector = t1;
    emitter = t2;
  } else {
    collector = t2;
    emitter = t1;
  }

  // Stima hFE
  float Vcc = 3.3f;
  float v_collettore = readVoltage(collector);
  float ic = (Vcc - v_collettore) / TP_SERIES_RESISTANCE;
  float ib = 0.00005f; // stima fissa
  float hfe = ic / ib;

  // Output
  String tipo = isNPN ? "NPN" : "PNP";
  String msg = "BJT: " + tipo + "\n";
  msg += "B=TP" + String(base + 1);
  msg += " C=TP" + String(collector + 1);
  msg += " E=TP" + String(emitter + 1);
  msg += "\nhFE ≈ " + String((int)hfe);

  showMessage(msg);
}

void test::detectCapacitor() {
  using namespace tp;
  using namespace adc;
  using namespace display;

  struct CapResult {
    int tp1, tp2;
    float capacitance_uF;
    float esr_ohm;
    bool polarized;
    bool valid;
  };

  auto measureCap = [](int a, int b) -> CapResult {
    CapResult result = {a, b, 0.0f, 0.0f, false, false};
    const float Vth = 1.1f;
    const float Vcc = 3.3f;
    const float R = TP_SERIES_RESISTANCE;

    // Scarica
    setMode(a, Mode::GND);
    setMode(b, Mode::GND);
    delay(20);

    // Carica
    setMode(a, Mode::OUT);
    write(a, HIGH);
    setMode(b, Mode::IN);

    unsigned long t0 = millis();
    float v = 0;
    while ((millis() - t0) < 1000) {
      v = readVoltage(b);
      if (v >= Vth) break;
    }
    unsigned long dt = millis() - t0;
    if (dt == 0 || v < Vth) return result;

    float C = (dt / 1000.0f) / R;
    float C_uF = C * 1e6f;
    if (C_uF < 0.1f || C_uF > 5000.0f) return result;

    // ESR
    setMode(a, Mode::GND);
    setMode(b, Mode::IN);
    delay(5);
    float vDrop = readVoltage(b);
    float i = Vcc / R;
    float esr = vDrop / i;

    // Polarità (grezza): tensione sale solo in una direzione
    bool polarized = false;
    if (vDrop < 0.3f) {
      // Inverti polarità e riprova
      setMode(b, Mode::OUT);
      write(b, HIGH);
      setMode(a, Mode::IN);
      delay(10);
      float vInv = readVoltage(a);
      if (vInv < 0.3f) polarized = true;
    }

    result.capacitance_uF = C_uF;
    result.esr_ohm = esr;
    result.polarized = polarized;
    result.valid = true;
    return result;
  };

  CapResult best = {};
  for (int i = 0; i < 3; ++i) {
    for (int j = i + 1; j < 3; ++j) {
      CapResult r = measureCap(i, j);
      if (r.valid && r.capacitance_uF > best.capacitance_uF)
        best = r;
    }
  }

  if (!best.valid) {
    showMessage("Nessun condensatore rilevato");
    return;
  }

  String msg = "Condensatore\n";
  msg += "TP" + String(best.tp1 + 1) + " - TP" + String(best.tp2 + 1) + "\n";
  msg += String(best.capacitance_uF, 1) + " µF\n";
  msg += "ESR ≈ " + String(best.esr_ohm, 1) + " Ω";
  if (best.polarized) msg += "\nPolarizzato";

  showMessage(msg);
}

void test::detectInductor() {
  using namespace tp;
  using namespace adc;
  using namespace display;

  struct InductanceResult {
    int tp1, tp2;
    float inductance_H;
    float resistance_ohm;
    bool valid;
  };

  auto measureInductor = [](int a, int b) -> InductanceResult {
    InductanceResult result = {a, b, 0.0f, 0.0f, false};
    const float Vcc = 3.3f;
    const float V_trigger = 1.0f;
    const float R = TP_SERIES_RESISTANCE;

    // Scarica
    setMode(a, Mode::GND);
    setMode(b, Mode::GND);
    delay(20);

    // Carica tramite R: A = HIGH, B = IN
    setMode(a, Mode::OUT);
    write(a, HIGH);
    setMode(b, Mode::IN);

    unsigned long t0 = micros();
    float v = 0;
    while ((micros() - t0) < 300000) {
      v = readVoltage(b);
      if (v >= V_trigger) break;
    }
    unsigned long t1 = micros();
    unsigned long delta_t_us = t1 - t0;
    if (delta_t_us < 100 || delta_t_us > 250000) return result;

    float time_s = delta_t_us / 1e6f;
    float L = R * time_s;  // H

    // Stima Rdc
    setMode(a, Mode::OUT);
    write(a, HIGH);
    setMode(b, Mode::GND);
    delay(10);
    setMode(a, Mode::IN);
    delayMicroseconds(50);
    float vDrop = readVoltage(a);
    float i = Vcc / R;
    float Rdc = (Vcc - vDrop) / i;

    result.inductance_H = L;
    result.resistance_ohm = Rdc;
    result.valid = true;
    return result;
  };

  InductanceResult best = {};
  for (int i = 0; i < 3; ++i) {
    for (int j = i + 1; j < 3; ++j) {
      InductanceResult r = measureInductor(i, j);
      if (r.valid && r.inductance_H > best.inductance_H)
        best = r;
    }
  }

  if (!best.valid) {
    showMessage("Nessun induttore rilevato");
    return;
  }

  String msg = "Induttore\nTP" + String(best.tp1 + 1) + " - TP" + String(best.tp2 + 1) + "\n";
  msg += String(best.inductance_H * 1000.0f, 2) + " mH\n";
  msg += "Rdc ≈ " + String(best.resistance_ohm, 1) + " Ω";
  showMessage(msg);
}

void test::detectMOSFET() {
  using namespace tp;
  using namespace adc;
  using namespace display;

  struct MosfetResult {
    int gate, drain, source;
    bool isNchannel;
    float rdson;
    bool valid;
  };

  auto detectDiode = [](int a, int b) -> bool {
    float v1 = tp::detectDiodeBetween(a, b);
    float v2 = tp::detectDiodeBetween(b, a);
    return (v1 > 0.4f || v2 > 0.4f);
  };

  MosfetResult result = {0, 0, 0, true, 0.0f, false};

  for (int g = 0; g < 3; ++g) {
    int d = (g + 1) % 3;
    int s = (g + 2) % 3;

    if (!detectDiode(d, s)) continue;

    // Tenta NMOS
    setMode(g, Mode::OUT);
    write(g, HIGH);  // G=HIGH
    setMode(s, Mode::GND);
    setMode(d, Mode::IN);
    delay(5);

    float vN = readVoltage(d);
    bool condN = vN > 0.5f;

    // Tenta PMOS
    write(g, LOW);  // G=LOW
    setMode(s, Mode::VCC);
    setMode(d, Mode::IN);
    delay(5);

    float vP = readVoltage(d);
    bool condP = vP < 2.8f;

    if (condN || condP) {
      result.gate = g;
      result.drain = d;
      result.source = s;
      result.isNchannel = condN;
      result.valid = true;
      break;
    }
  }

  if (!result.valid) {
    showMessage("Nessun MOSFET rilevato");
    return;
  }

  // Calcolo Rdson
  setMode(result.gate, Mode::OUT);
  write(result.gate, result.isNchannel ? HIGH : LOW);
  setMode(result.source, result.isNchannel ? Mode::GND : Mode::VCC);
  setMode(result.drain, Mode::IN);
  delay(10);
  float vOut = readVoltage(result.drain);
  float i = 3.3f / TP_SERIES_RESISTANCE;
  float vds = result.isNchannel ? (3.3f - vOut) : vOut;
  float rds = vds / i;

  // Output
  String msg = (result.isNchannel ? "N-MOSFET\n" : "P-MOSFET\n");
  msg += "G=TP" + String(result.gate + 1);
  msg += " D=TP" + String(result.drain + 1);
  msg += " S=TP" + String(result.source + 1);
  msg += "\nRds(on) ≈ " + String(rds, 1) + " Ω";

  showMessage(msg);
}

void test::testSCR() {
    display::print("Test SCR");

    for (uint8_t i = 0; i < 3; ++i) {
        uint8_t A = i;
        uint8_t K = (i + 1) % 3;
        uint8_t G = (i + 2) % 3;

        dischargeAll();

        // Applica tensione tra A e K
        setHigh(A);
        setLow(K);
        delay(10);
        bool conductsBefore = readVoltage(K) > threshold;

        // Impulso su Gate
        pulse(G, 5);  // impulso breve
        delay(10);
        bool conductsAfter = readVoltage(K) > threshold;

        // Reset
        setLow(A);
        delay(5);

        if (!conductsBefore && conductsAfter) {
            display::print("SCR rilevato");
            display::print("A: TP" + String(A+1));
            display::print("K: TP" + String(K+1));
            display::print("G: TP" + String(G+1));
            return;
        }
    }

    display::print("Nessun SCR rilevato");
}

showMessage(msg);

void test::testTRIAC() {
    display::print("Test TRIAC");

    for (uint8_t i = 0; i < 3; ++i) {
        uint8_t A1 = i;
        uint8_t A2 = (i + 1) % 3;
        uint8_t G  = (i + 2) % 3;

        dischargeAll();

        // Applica tensione alternata simulata (es. inverti polarità)
        setHigh(A1);
        setLow(A2);
        delay(10);
        bool conductsBefore = readVoltage(A2) > threshold;

        // Impulso su Gate
        pulse(G, 5);
        delay(10);
        bool conductsAfter = readVoltage(A2) > threshold;

        // Inverti polarità
        setLow(A1);
        setHigh(A2);
        delay(10);
        bool conductsReverse = readVoltage(A1) > threshold;

        // Reset
        setLow(A1);
        setLow(A2);
        delay(5);

        if (!conductsBefore && conductsAfter && conductsReverse) {
            display::print("TRIAC rilevato");
            display::print("A1: TP" + String(A1+1));
            display::print("A2: TP" + String(A2+1));
            display::print("G:  TP" + String(G+1));
            return;
        }
    }

    display::print("Nessun TRIAC rilevato");
}
showMessage(msg);

void test::testZener() {
    display::print("Test Zener");

    pinMode(STEPUP_EN, OUTPUT);
    digitalWrite(STEPUP_EN, LOW);  // step-up disattivo

    for (uint8_t i = 0; i < 3; ++i) {
        uint8_t anodo  = i;
        uint8_t catodo = (i + 1) % 3;

        dischargeAll();

        // Test in diretta: anodo LOW, catodo HIGH
        setLow(anodo);
        setHigh(catodo);
        delay(10);
        float vf = readVoltage(anodo);  // tensione diretta

        // Test in inversa con step-up
        dischargeAll();
        setLow(anodo);
        digitalWrite(STEPUP_EN, HIGH);  // attiva boost
        setHigh(catodo);                // catodo verso boost
        delay(50);                      // tempo per stabilizzare

        float vz = readVoltage(anodo);  // tensione inversa (su anodo)

        digitalWrite(STEPUP_EN, LOW);   // disattiva boost
        dischargeAll();

        if (vf < 1.0 && vz > VZ_MIN && vz < VZ_MAX) {
            display::print("Zener rilevato");
            display::print("Anodo: TP" + String(anodo + 1));
            display::print("Catodo: TP" + String(catodo + 1));
            display::print("Vz ≈ " + String(vz, 1) + " V");
            return;
        }
    }

    display::print("Nessun Zener rilevato");
}

showMessage(msg);

}
