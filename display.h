#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>

namespace display {

    void begin();
    void showWelcomeScreen();
    void showVoltage(float voltage, const char* label);
    void clear();
    void showMessage(const char* msg, uint16_t color = 0xFFFF);  // Default: bianco

}

#endif // DISPLAY_H
