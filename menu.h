// menu.h
#ifndef MENU_H
#define MENU_H

#include <Arduino.h> // Per i tipi di base come bool, uint8_t

// Definisci i pin dei pulsanti. Questi dovrebbero essere in config.h
// Se non sono in config.h, dovrai aggiungerli lì o definirli qui.
// Per esempio, se usi GPIO 0, 1, 2 per UP, DOWN, SELECT:
#ifndef BUTTON_UP_PIN
#define BUTTON_UP_PIN    0
#endif
#ifndef BUTTON_DOWN_PIN
#define BUTTON_DOWN_PIN  1
#endif
#ifndef BUTTON_SELECT_PIN
#define BUTTON_SELECT_PIN 2
#endif


namespace menu {

    // Dichiarazione delle funzioni di callback per le voci del menu.
    // Queste funzioni verranno implementate in menu.cpp
    void startComponentTest();
    void showTesterStatus();
    void measureFrequency();
    void ddsOutput();
    void irAnalysis();
    void shutdownDevice();

    // Struttura per definire una singola voce di menu
    typedef struct {
        const char* text;             // Testo da visualizzare per la voce di menu
        void (*callback)();           // Puntatore alla funzione da chiamare quando la voce è selezionata
    } MenuItem;

    // Funzioni principali del menu
    void init();        // Inizializza i pulsanti
    void loop();        // Gestisce la logica del menu (input pulsanti, navigazione)
    void drawMenu();    // Disegna il menu sul display

} // namespace menu

#endif // MENU_H