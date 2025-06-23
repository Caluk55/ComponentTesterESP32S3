#include <SPI.h>     // <-- AGGIUNGI QUESTA RIGA
#include "display.h"
#include "menu.h"
// #include "test.h"

void setup() {
    display::init();
    display::showWelcomeScreen();
    delay(3000);
    menu::init();
}

void loop() {
    menu::loop();
}