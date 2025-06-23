//menu.cpp
#include "menu.h"
#include "display.h"
#include "test.h"
#include "config.h"

namespace menu {

// Array voci menu principale
MenuItem menuItems[] = {
    {"Test Componente",    startComponentTest},
    {"Stato Tester",       showTesterStatus},
    {"Misura Frequenza",   measureFrequency},
    {"Uscita DDS",         ddsOutput},
    {"Analisi IR",         irAnalysis},
    {"Spegnimento",        shutdownDevice}
};
const int NUM_MENU_ITEMS = sizeof(menuItems) / sizeof(menuItems[0]);
int currentSelection = 0;

// Per debounce pulsanti
unsigned long lastButtonPressTime = 0;
const long DEBOUNCE_DELAY = 200;

void drawMenu() {
    display::clear();
    display::setTextSize(2);
    // Titolo centrato in alto
    const char* title = "Component Tester";
    int16_t tw = display::textWidth(title);
    int x0 = (display::screenWidth() - tw) / 2;
    display::setCursor(x0, 10);
    display::setTextColor(TFT_CYAN, TFT_BLACK);
    display::print(title);

    int startY = 30; // più in basso per lasciare spazio al titolo
    int lineHeight = display::fontHeight() + 5;

    for (int i = 0; i < NUM_MENU_ITEMS; i++) {
        int x = 10;
        int y = startY + (i * lineHeight);
        if (i == currentSelection) {
            display::setTextColor(TFT_BLACK, TFT_YELLOW);
            display::fillRect(0, y - 2, display::screenWidth(), lineHeight, TFT_YELLOW);
        } else {
            display::setTextColor(TFT_WHITE, TFT_BLACK);
        }
        display::setCursor(x, y);
        display::print(menuItems[i].text);
    }
}

void showNotImplemented() {
    display::clear();
    display::setTextSize(2);
    display::setCursor(10, 60);
    display::setTextColor(TFT_WHITE, TFT_BLACK);
    display::print("Funzione");
    display::setCursor(10, 90);
    display::print("non disponibile");
    display::setTextSize(1);
    display::setCursor(10, display::screenHeight() - 30);
    display::setTextColor(TFT_YELLOW, TFT_BLACK);
    display::print("Premi SELECT per tornare");

    // Aspetta che SELECT venga rilasciato (se era già premuto)
    while (digitalRead(BUTTON_SELECT_PIN) == LOW) delay(10);

    // Ora aspetta che venga premuto e poi rilasciato
    while (digitalRead(BUTTON_SELECT_PIN) == HIGH) delay(10);
    delay(DEBOUNCE_DELAY);
    while (digitalRead(BUTTON_SELECT_PIN) == LOW) delay(10);

    drawMenu();
}

void startComponentTest() {
    Serial.println("Avvio Test Componente...");
    display::clear();
    display::setTextSize(2);
    display::setCursor(10, 10);
    display::print("Test in corso...");
    delay(1000);
    test::runAll();
    display::setCursor(10, display::screenHeight() - 30);
    display::print("Premi SELECT per tornare");
    while (digitalRead(BUTTON_SELECT_PIN) == HIGH) delay(10);
    delay(DEBOUNCE_DELAY);
    while (digitalRead(BUTTON_SELECT_PIN) == LOW) delay(10);
    drawMenu();
}

void showTesterStatus() {
    showNotImplemented();
}
void measureFrequency() {
    showNotImplemented();
}
void ddsOutput() {
    showNotImplemented();
}
void irAnalysis() {
    showNotImplemented();
}

void shutdownDevice() {
    Serial.println("Spegnimento del dispositivo...");
    display::clear();
    display::setTextSize(2);
    display::setCursor(10, 10);
    display::print("Spegnimento...");
    delay(1000);
    display::clear(); // Schermo spento
    // Qui puoi aggiungere la logica reale di spegnimento hardware, se necessaria (deep sleep ecc.)
}

void init() {
    pinMode(BUTTON_UP_PIN, INPUT_PULLUP);
    pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);
    pinMode(BUTTON_SELECT_PIN, INPUT_PULLUP);
    drawMenu();
}

void loop() {
    unsigned long currentTime = millis();
    bool buttonPressed = false;

    // Pulsante SU
    if (digitalRead(BUTTON_UP_PIN) == LOW && currentTime - lastButtonPressTime > DEBOUNCE_DELAY) {
        currentSelection--;
        if (currentSelection < 0) currentSelection = NUM_MENU_ITEMS - 1;
        lastButtonPressTime = currentTime;
        buttonPressed = true;
    }

    // Pulsante GIU
    if (digitalRead(BUTTON_DOWN_PIN) == LOW && currentTime - lastButtonPressTime > DEBOUNCE_DELAY) {
        currentSelection++;
        if (currentSelection >= NUM_MENU_ITEMS) currentSelection = 0;
        lastButtonPressTime = currentTime;
        buttonPressed = true;
    }

    // Pulsante SELECT
    if (digitalRead(BUTTON_SELECT_PIN) == LOW && currentTime - lastButtonPressTime > DEBOUNCE_DELAY) {
        lastButtonPressTime = currentTime;
        buttonPressed = true;
        if (menuItems[currentSelection].callback != nullptr) {
            menuItems[currentSelection].callback();
        }
    }

    if (buttonPressed) {
        drawMenu();
    }
}

} // namespace menu