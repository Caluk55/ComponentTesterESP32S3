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




}
