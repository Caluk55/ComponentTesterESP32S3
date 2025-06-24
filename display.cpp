#include "display.h"
#include "config.h"  // DEVE essere incluso prima della classe LGFX per DISPLAY_WIDTH/HEIGHT
#include "icons.h"   // Per SymbolData e le costanti relative ai simboli
#include <LovyanGFX.hpp>
#include <SPI.h> // Mantenuto, risolveva l'errore di linker

// *** Configurazione LovyanGFX per ESP32S3 + ST7789 ***
// IDEALMENTE: Questa classe LGFX e l'istanza tft dovrebbero stare in LGFX_Config.hpp
// e non qui in display.cpp. Ma per far compilare il tuo setup attuale,
// l'ho lasciata qui con l'inclusione di config.h spostata correttamente.
class LGFX : public lgfx::LGFX_Device {
  lgfx::Panel_ST7789 _panel;
  lgfx::Bus_SPI _bus;
public:
  LGFX(void) {
    { // BUS SPI
      auto cfg = _bus.config();
      cfg.spi_host = SPI3_HOST;
      cfg.spi_mode = 3;
      cfg.freq_write = 40000000;
      cfg.freq_read  = 16000000;
      cfg.spi_3wire  = false;
      cfg.pin_sclk = 12;
      cfg.pin_mosi = 11;
      cfg.pin_miso = -1;
      cfg.pin_dc    = 9;
      _bus.config(cfg);
      _panel.setBus(&_bus);
    }
    { // PANEL
      auto cfg = _panel.config();
      cfg.pin_cs      = 10;
      cfg.pin_rst     = 8;
      cfg.pin_busy    = -1;
      cfg.panel_width    = DISPLAY_WIDTH; // Ora dovrebbe essere riconosciuto da config.h
      cfg.panel_height   = DISPLAY_HEIGHT; // Ora dovrebbe essere riconosciuto da config.h
      cfg.memory_width  = DISPLAY_WIDTH;
      cfg.memory_height = DISPLAY_HEIGHT;
      cfg.offset_x      = 0;
      cfg.offset_y      = 0;
      cfg.offset_rotation = 0;
      cfg.dummy_read_pixel = 8;
      cfg.dummy_read_bits   = 1;
      cfg.readable = false;
      cfg.invert    = false;
      cfg.rgb_order = false;
      _panel.config(cfg);
    }
    setPanel(&_panel);
  }
};

static LGFX tft; // Istanza globale del display
namespace display {
static LGFX_Sprite iconSprite(&tft); // Sprite per le icone

void init() {
    tft.init();
    tft.setRotation(3); // Rotazione del display
    tft.fillScreen(TFT_BLACK);
}

void clear() {
    tft.fillScreen(TFT_BLACK);
}

void showWelcomeScreen() {
    clear();
    int centerX = tft.width() / 2;
    // int centerY = tft.height() / 2; // Non usata direttamente nel calcolo attuale

    tft.setTextSize(3);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);

    const char* title = "Component Tester";
    int titleWidth = tft.textWidth(title);
    int titleHeight = tft.fontHeight();

    // --- Calcolo dell'icona ---
    int scaleFactor = 2; // Il tuo fattore di scalatura
    int iconWidthScaled = SYMBOL_SIZE_X * scaleFactor;
    int iconHeightScaled = SYMBOL_SIZE_Y * scaleFactor;

    int iconTopMargin = 10; // Margine superiore dello schermo per l'icona
    int textIconSpacing = 15; // Spazio tra l'icona e il titolo

    int iconY = iconTopMargin;
    int iconX = centerX - (iconWidthScaled / 2); // Icona centrata orizzontalmente

    // La Y del titolo sarà sotto l'icona, più uno spazio
    int titleY = iconY + iconHeightScaled + textIconSpacing;
    int titleX = centerX - (titleWidth / 2); // Titolo centrato orizzontalmente

    tft.setCursor(titleX, titleY);
    tft.println(title);

    // Ora disegniamo l'icona, con le coordinate calcolate
    showIcon(0, iconX, iconY, TFT_WHITE, scaleFactor); // Chiamata corretta

    // *** NUOVO: Aggiungiamo "Cal 2025" sotto il titolo ***
    tft.setTextSize(2); // Dimensione più piccola per la data
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    const char* calDate = "Cal 2025";
    int calDateWidth = tft.textWidth(calDate);
    int calDateHeight = tft.fontHeight();

    int calDateY = titleY + titleHeight + 5; // Sotto il titolo, con un piccolo margine
    int calDateX = centerX - (calDateWidth / 2); // Centrato orizzontalmente

    tft.setCursor(calDateX, calDateY);
    tft.println(calDate);
}

void showVoltage(float voltage, const char* tp_label, int y) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%s: %.2f V", tp_label, voltage);
    tft.setFont(&fonts::Font2);
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.setTextSize(2);

    int startX = 20;
    int lineHeight = tft.fontHeight() + 5; // Usa tft.fontHeight() qui

    if (strcmp(tp_label, "TP1") == 0) {
        y = 10;
    } else if (strcmp(tp_label, "TP2") == 0) {
        y = 10 + lineHeight;
    } else if (strcmp(tp_label, "TP3") == 0) {
        y = 10 + (2 * lineHeight);
    } else {
        y = (y < 0) ? tft.getCursorY() + 30 : y;
    }

    tft.setCursor(startX, y);
    tft.println(buf);
}

void showMessage(const char* msg, int y) {
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setFont(&fonts::Font2);
    tft.setTextSize(2);
    if (y < 0) y = tft.getCursorY() + 30; // Se y è negativo, posiziona sotto l'ultima riga
    tft.setCursor(20, y);
    tft.println(msg);
}

// Funzione per mostrare un'icona con scalatura manuale su Sprite
void showIcon(uint8_t symbol_idx, int x, int y, uint16_t color, int scaleFactor) {
    const uint8_t* iconData = &SymbolData[symbol_idx * SYMBOL_BYTES_N];

    int bytesPerRow = SYMBOL_BYTES_X; // Manteniamo il valore normale (4)

    int scaledWidth = SYMBOL_SIZE_X * scaleFactor;
    int scaledHeight = SYMBOL_SIZE_Y * scaleFactor;

    if (iconSprite.width() != scaledWidth || iconSprite.height() != scaledHeight) {
        iconSprite.deleteSprite();
        iconSprite.createSprite(scaledWidth, scaledHeight);
    }

    iconSprite.fillScreen(0x0000); // Pulisci lo sprite

    for (int currentY = 0; currentY < SYMBOL_SIZE_Y; ++currentY) {
        for (int currentX = 0; currentX < SYMBOL_SIZE_X; ++currentX) {
            uint8_t byteIndex = (currentY * bytesPerRow) + (currentX / 8);
            uint8_t bitMask = (1 << (currentX % 8)); // LSB-first
            if ((iconData[byteIndex] & bitMask) != 0) {
                iconSprite.fillRect(currentX * scaleFactor, currentY * scaleFactor,
                                    scaleFactor, scaleFactor, color);
            }
        }
    }

    iconSprite.pushSprite(x, y);
}

// Implementazioni delle funzioni dichiarate in display.h e usate da menu.cpp
void setTextColor(uint32_t fg, uint32_t bg) {
    tft.setTextColor(fg, bg);
}

void fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color) {
    tft.fillRect(x, y, w, h, color);
}

void setCursor(int32_t x, int32_t y) {
    tft.setCursor(x, y);
}

void print(const char* text) {
    tft.print(text);
}

void print(int value) {
    tft.print(value);
}

void setTextSize(uint8_t size) {
    tft.setTextSize(size);
}
// AGGIUNGI ALLA FINE DEL NAMESPACE display:

int fontHeight() {
    return tft.fontHeight();
}
int screenWidth() {
    return tft.width();
}
int screenHeight() {
    return tft.height();
}
  int16_t textWidth(const char* text) {
    return tft.textWidth(text); // o il metodo equivalente di LovyanGFX
}
  

} // fine namespace display