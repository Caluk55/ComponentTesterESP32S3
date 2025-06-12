#include "display.h"
#include "icons.h"

// Crea l'istanza globale del display (adatta se necessario)
TFT_eSPI tft = TFT_eSPI();

namespace display {

void init() {
    tft.init();
    tft.setRotation(1); // Adatta in base al montaggio fisico
    tft.fillScreen(TFT_BLACK);
}

void clear() {
    tft.fillScreen(TFT_BLACK);
}

void showWelcomeScreen() {
    clear();
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(20, 40);
    tft.println("Component Tester");
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(1);
    tft.setCursor(20, 70);
    tft.println("by Cal 2025");
    // Eventuale logo/icone
    showIcon(0, 180, 10, TFT_GREEN); // esempio, icona 0
}

void showVoltage(float voltage, const char* tp_label, int y) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%s: %.2f V", tp_label, voltage);
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.setTextSize(2);
    if (y < 0) y = tft.getCursorY() + 30; // posizione automatica
    tft.setCursor(20, y);
    tft.println(buf);
}

void showMessage(const char* msg, int y) {
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    if (y < 0) y = tft.getCursorY() + 30;
    tft.setCursor(20, y);
    tft.println(msg);
}

void showIcon(uint8_t symbol_idx, int x, int y, uint16_t color) {
    // Disegna una bitmap 32x39 (156 byte) da SymbolData
    tft.drawBitmap(x, y, &SymbolData[symbol_idx * SYMBOL_BYTES_N], SYMBOL_SIZE_X, SYMBOL_SIZE_Y, color);
}

} // namespace display