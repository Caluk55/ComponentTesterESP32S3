#ifndef DISPLAY_H
#define DISPLAY_H

#include "config.h"

// Se usi TFT_eSPI o una libreria specifica, includila qui
#include <TFT_eSPI.h>
#include <stdint.h>


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

// Mostra una icona (indice, posizione, colore)
void showIcon(uint8_t symbol_idx, int x, int y, uint16_t color);

} // namespace display

#endif // DISPLAY_H