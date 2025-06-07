// display.h
#ifndef _DISPLAY_H_
#define _DISPLAY_H_

// Includere solo header locali o di sistema strettamente necessari per le DICHIARAZIONI.
// Evitare TFTeSPI.h o Arduino.h qui.

// Dichiara che 'tft' è definita altrove (nel .ino)
extern TFT_eSPI tft;

// Dimensioni delle icone (devono corrispondere a quelle effettive degli array)
#define ICON_WIDTH  32
#define ICON_HEIGHT 39

// Dichiarazioni di funzioni display
void displayWelcomeScreen();
void drawComponentIcon(byte componentType, int x, int y, int numPins);
void drawComponentPins(int centerX, int startY, int numPins);
void displayMeasurementResult(const char* label, const char* value, int x, int y, int textColor);
// Aggiungere altre funzioni display qui man mano che servono

#endif // _DISPLAY_H_