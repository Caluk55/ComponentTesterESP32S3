// menu.cpp
#include "menu.h"
#include "display.h" // Per interagire con il display (display::clear, display::print, ecc.)
#include "test.h"    // Per chiamare la funzione di test del componente (test::runAllTests)

// Dichiarazione dell'istanza tft che è definita in display.cpp
// Questo ci permette di usare metodi di tft direttamente qui se necessario.
extern LGFX tft;

// Definizione delle voci del menu con il loro testo e la funzione associata.
MenuItem menuItems[] = {
    {"Test Componente", menu::startComponentTest},
    {"Stato Tester",    menu::showTesterStatus},
    {"Misura Frequenza", menu::measureFrequency},
    {"Uscita DDS",      menu::ddsOutput},
    {"Analisi IR",      menu::irAnalysis},
    {"Spegnimento",     menu::shutdownDevice}
};

const int NUM_MENU_ITEMS = sizeof(menuItems) / sizeof(menuItems[0]); // Numero totale di voci

int currentSelection = 0; // Indice della voce di menu attualmente selezionata

// Variabili per il debounce dei pulsanti
unsigned long lastButtonPressTime = 0;
const long DEBOUNCE_DELAY = 200; // Millisecondi di ritardo per il debounce

namespace menu {

void init() {
    // Configura i pin dei pulsanti come INPUT_PULLUP
    pinMode(BUTTON_UP_PIN, INPUT_PULLUP);
    pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);
    pinMode(BUTTON_SELECT_PIN, INPUT_PULLUP);
}

void loop() {
    unsigned long currentTime = millis();
    bool buttonPressed = false;

    // --- Gestione del pulsante "SU" ---
    if (digitalRead(BUTTON_UP_PIN) == LOW && currentTime - lastButtonPressTime > DEBOUNCE_DELAY) {
        currentSelection--;
        if (currentSelection < 0) {
            currentSelection = NUM_MENU_ITEMS - 1;
        }
        lastButtonPressTime = currentTime;
        buttonPressed = true;
    }

    // --- Gestione del pulsante "GIÙ" ---
    if (digitalRead(BUTTON_DOWN_PIN) == LOW && currentTime - lastButtonPressTime > DEBOUNCE_DELAY) {
        currentSelection++;
        if (currentSelection >= NUM_MENU_ITEMS) {
            currentSelection = 0;
        }
        lastButtonPressTime = currentTime;
        buttonPressed = true;
    }

    // --- Gestione del pulsante "SELEZIONA" ---
    if (digitalRead(BUTTON_SELECT_PIN) == LOW && currentTime - lastButtonPressTime > DEBOUNCE_DELAY) {
        lastButtonPressTime = currentTime;
        buttonPressed = true;

        // Se la funzione di callback esiste, chiamala
        if (menuItems[currentSelection].callback != nullptr) {
            menuItems[currentSelection].callback();
        }
    }

    // Se un pulsante è stato premuto, ridisegna il menu
    if (buttonPressed) {
        drawMenu();
    }
}

// Funzione per disegnare il menu sul display
void drawMenu() {
    display::clear();
    display::setTextSize(2); // Dimensione testo per le voci del menu

    int startY = 10;
    // tft.fontHeight() restituisce l'altezza del font corrente
    int lineHeight = tft.fontHeight() + 5; // Altezza di ogni riga + spazio tra le voci

    for (int i = 0; i < NUM_MENU_ITEMS; i++) {
        int x = 10; // Margine sinistro
        int y = startY + (i * lineHeight);

        // Evidenzia la voce selezionata
        if (i == currentSelection) {
            display::setTextColor(TFT_BLACK, TFT_YELLOW); // Testo nero su sfondo giallo
            // Disegna un rettangolo giallo su tutta la larghezza per la voce selezionata
            display::fillRect(0, y - 2, tft.width(), lineHeight, TFT_YELLOW);
        } else {
            display::setTextColor(TFT_WHITE, TFT_BLACK); // Testo bianco su sfondo nero
        }

        display::setCursor(x, y);
        display::print(menuItems[i].text);
    }
}

// --- Implementazioni delle funzioni di callback del menu ---
// Ogni funzione è un placeholder per la logica specifica.
// Dopo aver eseguito la sua logica, dovrebbe aspettare l'input
// dell'utente per tornare al menu.

void startComponentTest() {
    Serial.println("Avvio Test Componente...");
    display::clear();
    display::setTextSize(2);
    display::setCursor(10, 10);
    display::print("Test in corso...");
    delay(1000); // Simula un breve delay prima del test effettivo

    // CHIAMA LA TUA FUNZIONE DI TEST PRINCIPALE QUI
    // Ad esempio: test::runAllTests();
    // Assicurati che 'test::runAllTests()' sia accessibile e faccia il lavoro.
    // Per ora, userò un placeholder:
    test::runAllTests(); // Questa funzione è dichiarata in test.h e implementata in test.cpp

    display::setCursor(10, tft.height() - 30); // Posiziona in basso per istruzioni
    display::print("Premi SELECT per tornare");
    // Aspetta che il pulsante SELECT venga premuto e poi rilasciato
    while (digitalRead(BUTTON_SELECT_PIN) == HIGH) {
        delay(10); // Piccolo delay per non saturare la CPU
    }
    delay(DEBOUNCE_DELAY); // Debounce
    while (digitalRead(BUTTON_SELECT_PIN) == LOW) {
        delay(10); // Attendi il rilascio del pulsante
    }
    // Dopo l'esecuzione e il feedback dell'utente, torna al menu principale
    // Il menu verrà ridisegnato dal menu::loop() quando buttonPressed sarà true
    // o dalla chiamata a drawMenu() se torniamo direttamente.
}

void showTesterStatus() {
    Serial.println("Visualizzazione Stato Tester...");
    display::clear();
    display::setTextSize(2);
    display::setCursor(10, 10);
    display::print("Stato Tester:");
    display::setCursor(10, 40);
    display::print("TP1: X.XXV"); // Placeholder, da aggiornare con valori reali
    display::setCursor(10, 60);
    display::print("Batteria: OK"); // Placeholder, da aggiornare con valori reali

    display::setCursor(10, tft.height() - 30);
    display::print("Premi SELECT per tornare");
    while (digitalRead(BUTTON_SELECT_PIN) == HIGH) {
        delay(10);
    }
    delay(DEBOUNCE_DELAY);
    while (digitalRead(BUTTON_SELECT_PIN) == LOW) {
        delay(10);
    }
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
    while (digitalRead(BUTTON_SELECT_PIN) == HIGH) {
        delay(10);
    }
    delay(DEBOUNCE_DELAY);
    while (digitalRead(BUTTON_SELECT_PIN) == LOW) {
        delay(10);
    }
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
    while (digitalRead(BUTTON_SELECT_PIN) == HIGH) {
        delay(10);
    }
    delay(DEBOUNCE_DELAY);
    while (digitalRead(BUTTON_SELECT_PIN) == LOW) {
        delay(10);
    }
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
    while (digitalRead(BUTTON_SELECT_PIN) == HIGH) {
        delay(10);
    }
    delay(DEBOUNCE_DELAY);
    while (digitalRead(BUTTON_SELECT_PIN) == LOW) {
        delay(10);
    }
    drawMenu();
}

void shutdownDevice() {
    Serial.println("Spegnimento del dispositivo...");
    display::clear();
    display::setTextSize(2);
    display::setCursor(10, 10);
    display::print("Spegnimento...");
    delay(1000);
    tft.fillScreen(TFT_BLACK); // Spegni lo schermo (o imposta in modalità a basso consumo)
    // Qui potresti aggiungere la logica per spegnere effettivamente il dispositivo,
    // ad esempio, disattivare l'alimentazione a componenti specifici.
}

} // fine namespace menu