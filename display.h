#ifndef DISPLAY_H
#define DISPLAY_H

#include "config.h"    // Usa virgolette per i tuoi file locali
#include <Arduino.h>
#include <LovyanGFX.hpp> // Assicurati che LovyanGFX.hpp sia incluso correttamente

// Nota: 'extern LGFX tft;' non è qui. 
// Se LGFX è definita in LGFX_Config.hpp e tft è istanziata lì, 
// main.ino dovrà includere LGFX_Config.hpp o dichiarare 'extern LGFX tft;'.
// Per ora, assumiamo che LGFX_Config.hpp o display.cpp (come nel tuo caso) 
// gestisca l'istanza tft per tutto il progetto.

namespace display {

// Inizializza il display
void init();

// Pulisce lo schermo
void clear();

// Mostra una schermata di benvenuto
void showWelcomeScreen();

// Mostra la tensione misurata su un TP (es: "TP1: 1.23V")
void showVoltage(float voltage, const char* tp_label, int y = -1);

// Mostra un messaggio generico
void showMessage(const char* msg, int y = -1);

// Mostra un'icona (indice, posizione, colore, fattore di scala)
// Ho aggiunto 'scaleFactor' qui per corrispondere all'implementazione in display.cpp
void showIcon(uint8_t symbol_idx, int x, int y, uint16_t color, int scaleFactor);

// Funzioni aggiuntive per il disegno, necessarie per menu.cpp
void setTextColor(uint32_t fg, uint32_t bg = 0);
void fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);
void setCursor(int32_t x, int32_t y);
void print(const char* text);
void print(int value);
void setTextSize(uint8_t size);
int fontHeight();
int screenWidth();
int screenHeight();
int16_t textWidth(const char* text); 

} // namespace display

#endif // DISPLAY_H