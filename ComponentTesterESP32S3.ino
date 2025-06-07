#include <TFT_eSPI.h>

// ComponentTester_ESP32S3.ino
// Progetto Component Tester adattato per ESP32S3 con display TFT ST7789 e libreria TFTeSPI.

// --- Dichiarazione delle librerie ---
//#include <TFTeSPI.h>   // Questa DEVE essere la prima, dato il comportamento osservato
#include <Arduino.h>   // Subito dopo

// --- Inclusione degli header dei moduli personalizzati ---
#include "config.h"      // File di configurazione generale (già OK)
#include "colors.h"      // Definizione colori (già OK)
#include "display.h"     // Dichiarazioni funzioni display (Il TUO display.h)
#include "icons.h"       // Per gli array di icone

// --- Dichiarazione GLOBALE dell'oggetto TFTeSPI ---
TFT_eSPI tft; // Dichiarazione: l'oggetto è qui e unico

void setup() {
  Serial.begin(115200); // Per debug, utile
  tft.init();
  tft.setRotation(1); // Rotazione del display (1 per 240x320 orizzontale, 0 per 320x240 verticale)
  tft.fillScreen(TFT_BLACK);

  // Esempio: Mostra la schermata di benvenuto all'avvio
  displayWelcomeScreen();
  delay(2000); // Mostra per 2 secondi
  tft.fillScreen(TFT_BLACK); // Pulisci lo schermo

  // Esempio: Disegna un'icona e i pin numerati
  drawComponentIcon(COMPONENT_RESISTOR, 50, 50, 3); // Disegna icona resistore a 50,50 con 3 pin
  drawComponentPins(50 + ICON_WIDTH / 2, 50 + ICON_HEIGHT, 3); // Disegna i pin sotto l'icona
}

void loop() {
  // Qui andrà la logica principale del tester
  // Per ora, possiamo lasciare vuoto o aggiungere un loop di test
  delay(100);
}