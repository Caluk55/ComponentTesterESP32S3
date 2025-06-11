#include "display.h"
#include "config.h"
#include <TFT_eSPI.h>  // Assicurati che la libreria sia installata

TFT_eSPI tft = TFT_eSPI();  // Usa le impostazioni da User_Setup.h

namespace display {

    void begin() {
        tft.init();
        tft.setRotation(1);
        tft.fillScreen(TFT_BLACK);
        tft.setTextSize(2);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
    }

    void showWelcomeScreen() {
        tft.fillScreen(TFT_BLACK);
        tft.setCursor(10, DISPLAY_HEIGHT / 2 - 20);
        tft.setTextColor(TFT_CYAN);
        tft.setTextSize(2);
        tft.println("Component Tester");
        tft.setCursor(10, DISPLAY_HEIGHT / 2 + 10);
        tft.setTextColor(TFT_YELLOW);
        tft.println("ESP32-S3 Ready");
        delay(1500);
    }

    void showVoltage(float voltage, const char* label) {
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
        tft.setCursor(10, 100);
        tft.printf("%s: %.2f V\n", label, voltage);
    }

    void showMessage(const char* msg, uint16_t color) {
        tft.setTextColor(color, TFT_BLACK);
        tft.setCursor(10, 140);
        tft.println(msg);
    }

    void clear() {
        tft.fillScreen(TFT_BLACK);
    }

}
