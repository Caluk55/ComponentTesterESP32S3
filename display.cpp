// display.cpp
#include "display.h"   // Include il proprio header per le dichiarazioni e per extern TFT_eSPI tft;
#include "icons.h"     // Per accedere agli array delle icone
#include "colors.h"    // Per i colori
#include "config.h"    // Per costanti globali come FONT_CHAR_WIDTH, FONT_CHAR_HEIGHT

// Implementazione della schermata di benvenuto
void displayWelcomeScreen() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor( (tft.width() - 25 * 12) / 2, tft.height() / 2 - 20 ); // Centra il testo
  tft.println("Component Tester");
  tft.setCursor( (tft.width() - 15 * 12) / 2, tft.height() / 2 + 10 );
  tft.println("Powered by ESP32S3");
}

// Funzione per disegnare l'icona del componente
void drawComponentIcon(byte componentType, int x, int y, int numPins) {
  const unsigned char* iconData = nullptr;

  switch (componentType) {
    case COMPONENT_RESISTOR:
      iconData = resistor_icon;
      break;
    case COMPONENT_CAPACITOR:
      iconData = capacitor_icon;
      break;
    case COMPONENT_DIODE:
      iconData = diode_icon;
      break;
    // Aggiungi altri case per gli altri tipi di componente
    default:
      // Se il tipo non è riconosciuto, disegna un rettangolo vuoto o un'icona di errore
      tft.drawRect(x, y, ICON_WIDTH, ICON_HEIGHT, TFT_RED);
      return;
  }

  // Disegna il bitmap dell'icona (TFTeSPI.pushImage richiede i colori a 16 bit)
  // Assumi che gli array icona siano in formato 16 bit (colore alto, colore basso)
  // Se i tuoi array icona sono a 1 bit, avrai bisogno di una funzione custom per disegnare.
  // Per ora, assumiamo che siano come nell'originale ma riadattati a 16-bit per pushImage.
  // Se sono a 1 bit (bianco/nero), dovremmo scrivere una routine che li converte in runtime
  // o che li disegna pixel per pixel con tft.drawPixel.
  // Se non hai gli array come 16-bit, dovremo vederli.

  // Per iniziare, useremo drawXBitmap, che è più adatto per bitmap a 1 bit.
  // Questo richiede che le icone siano nel formato XBM (X-Bitmap)
  // Se i tuoi array sono come quelli originali (1-bit B/W), usa drawXBitmap:
  tft.drawXBitmap(x, y, (const uint8_t*)iconData, ICON_WIDTH, ICON_HEIGHT, TFT_WHITE, TFT_BLACK);

  // O se fossero dati come array di colori 16 bit (PROGMEM)
  // tft.pushImage(x, y, ICON_WIDTH, ICON_HEIGHT, (const uint16_t*)iconData);

  // Aggiungiamo un contorno per estetica e per debug
  tft.drawRect(x, y, ICON_WIDTH, ICON_HEIGHT, TFT_DARKGREY);
}


// Funzione per disegnare i quadratini numerati dei pin
void drawComponentPins(int centerX, int startY, int numPins) {
  int pinBoxSize = 15; // Dimensione del quadrato del pin
  int spacing = 5;     // Spazio tra i quadratini
  int totalWidth = (numPins * pinBoxSize) + ((numPins - 1) * spacing);
  int startX = centerX - (totalWidth / 2);

  tft.setTextSize(1); // Testo più piccolo per i numeri dei pin

  for (int i = 0; i < numPins; i++) {
    int x = startX + i * (pinBoxSize + spacing);
    int y = startY + 5; // Un piccolo offset sotto l'icona

    tft.drawRect(x, y, pinBoxSize, pinBoxSize, TFT_WHITE); // Bordo del quadratino
    tft.fillRect(x + 1, y + 1, pinBoxSize - 2, pinBoxSize - 2, TFT_DARKGREY); // Riempimento

    // Centra il numero all'interno del quadratino
    String pinNum = String(i + 1);
    int textX = x + (pinBoxSize - tft.textWidth(pinNum)) / 2;
    int textY = y + (pinBoxSize - FONT_CHAR_HEIGHT) / 2; // FONT_CHAR_HEIGHT da config.h

    tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
    tft.setCursor(textX, textY);
    tft.print(pinNum);
  }
}

// Funzione per visualizzare il risultato di una misurazione
void displayMeasurementResult(const char* label, const char* value, int x, int y, int textColor) {
  tft.setTextColor(textColor, TFT_BLACK);
  tft.setTextSize(2); // Dimensione testo per i risultati
  tft.setCursor(x, y);
  tft.print(label);
  tft.print(": ");
  tft.print(value);
}