// menu.cpp
#include "menu.h"
#include "display.h" // Per interagire con il display
#include "test.h"    // Per chiamare la funzione di test del componente

extern LGFX tft;

// Array voci menu principale
menu::MenuItem menuItems[] = {
    {"Test Componente", menu::startComponentTest},
    {"Stato Tester",    menu::showTesterStatus},
    {"Misura Frequenza", menu::measureFrequency},
    {"Uscita DDS",      menu::ddsOutput},
    {"Analisi IR",      menu::irAnalysis},
    {"Spegnimento",     menu::shutdownDevice}
};
const int NUM_MENU_ITEMS = sizeof(menuItems) / sizeof(menuItems[0]);
int currentSelection = 0;

// Per debounce pulsanti
unsigned long lastButtonPressTime = 0;
const long DEBOUNCE_DELAY = 200;

namespace menu {

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

// Disegna il menu principale
void drawMenu() {
    display::clear();
    display::setTextSize(2);
    int startY = 10;
    int lineHeight = tft.fontHeight() + 5;

    for (int i = 0; i < NUM_MENU_ITEMS; i++) {
        int x = 10;
        int y = startY + (i * lineHeight);
        if (i == currentSelection) {
            display::setTextColor(TFT_BLACK, TFT_YELLOW);
            display::fillRect(0, y - 2, tft.width(), lineHeight, TFT_YELLOW);
        } else {
            display::setTextColor(TFT_WHITE, TFT_BLACK);
        }
        display::setCursor(x, y);
        display::print(menuItems[i].text);
    }
}

// --- Callback delle voci del menu ---

void startComponentTest() {
    Serial.println("Avvio Test Componente...");
    display::clear();
    display::setTextSize(2);
    display::setCursor(10, 10);
    display::print("Test in corso...");
    delay(1000);

    // Esegui il test (placeholder)
    test::runAllTests();

    display::setCursor(10, tft.height() - 30);
    display::print("Premi SELECT per tornare");
    // Attendi SELECT
    while (digitalRead(BUTTON_SELECT_PIN) == HIGH) delay(10);
    delay(DEBOUNCE_DELAY);
    while (digitalRead(BUTTON_SELECT_PIN) == LOW) delay(10);
    drawMenu();
}

void showTesterStatus() {
    Serial.println("Visualizzazione Stato Tester...");
    display::clear();
    display::setTextSize(2);
    display::setCursor(10, 10);
    display::print("Stato Tester:");

    // Esempio di dati placeholder
    display::setCursor(10, 40);
    display::print("TP1: X.XXV");
    display::setCursor(10, 60);
    display::print("Batteria: OK");

    display::setCursor(10, tft.height() - 30);
    display::print("Premi SELECT per tornare");
    while (digitalRead(BUTTON_SELECT_PIN) == HIGH) delay(10);
    delay(DEBOUNCE_DELAY);
    while (digitalRead(BUTTON_SELECT_PIN) == LOW) delay(10);
    drawMenu();
}

void measureFrequency() {
    Serial.println("Avvio Misura Frequenza...");
    display::clear();
    display::setTextSize(2);
    display::setCursor(10, 10);
    display::print("Misura Freq. In attesa...");

    display::setCursor(10, tft.height() - 30);
    display::print("Premi SELECT per tornare");
    while (digitalRead(BUTTON_SELECT_PIN) == HIGH) delay(10);
    delay(DEBOUNCE_DELAY);
    while (digitalRead(BUTTON_SELECT_PIN) == LOW) delay(10);
    drawMenu();
}

void ddsOutput() {
    Serial.println("Controllo Uscita DDS...");
    display::clear();
    display::setTextSize(2);
    display::setCursor(10, 10);
    display::print("DDS: Onda Quadra 1kHz");

    display::setCursor(10, tft.height() - 30);
    display::print("Premi SELECT per tornare");
    while (digitalRead(BUTTON_SELECT_PIN) == HIGH) delay(10);
    delay(DEBOUNCE_DELAY);
    while (digitalRead(BUTTON_SELECT_PIN) == LOW) delay(10);
    drawMenu();
}

void irAnalysis() {
    Serial.println("Avvio Analisi IR...");
    display::clear();
    display::setTextSize(2);
    display::setCursor(10, 10);
    display::print("Analisi IR: Rilevando...");

    display::setCursor(10, tft.height() - 30);
    display::print("Premi SELECT per tornare");
    while (digitalRead(BUTTON_SELECT_PIN) == HIGH) delay(10);
    delay(DEBOUNCE_DELAY);
    while (digitalRead(BUTTON_SELECT_PIN) == LOW) delay(10);
    drawMenu();
}

void shutdownDevice() {
    Serial.println("Spegnimento del dispositivo...");
    display::clear();
    display::setTextSize(2);
    display::setCursor(10, 10);
    display::print("Spegnimento...");
    delay(1000);
    tft.fillScreen(TFT_BLACK); // Schermo spento
    // Qui puoi aggiungere la logica reale di spegnimento hardware.
}

} // namespace menu