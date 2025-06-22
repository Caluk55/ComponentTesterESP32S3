#ifndef CONFIG_H
#define CONFIG_H

// === Parametri elettrici reali ===
#define TP_SERIES_RESISTANCE 680.0f        // Ω — Resistenza in serie per limitare la corrente sui Test Point
#define TP_PULL_RESISTANCE   470000.0f     // Ω — Pull-down fissa su ogni Test Point
#define TP_DIODE_DROP        0.65f         // V — Caduta tipica dei diodi di protezione

// === Debug ===
#define DEBUG 1
#define VERBOSE 1  // Stampa dettagliata su Serial

// === Pin Mapping ===
#define TP1_PIN 4
#define TP2_PIN 7  // MODIFICATO PER RISOLVERE IL CONFLITTO COL TFT CS
#define TP3_PIN 6
#define GPIO_STEPUP_EN  25

// === Pin Encoder / Pulsante ===
#define ENCODER_BUTTON_PIN 10 // PIN per il pulsante dell'encoder
#define ENCODER_PIN_A      11 // Pin A dell'encoder
#define ENCODER_PIN_B      12 // Pin B dell'encoder

// === ADC Settings ===
#define ADC_RESOLUTION 4095.0f  // 12-bit
#define ADC_VREF 3.3f           // Tensione di riferimento ADC

// === Soglie di Test ===
#define VOLTAGE_THRESHOLD_LOW  0.2f
#define VOLTAGE_THRESHOLD_HIGH 3.0f

// === Display Settings ===
#define DISPLAY_WIDTH  240
#define DISPLAY_HEIGHT 320

#endif // CONFIG_H

/* # Promemoria Pinout Component Tester

Questo documento riassume l'assegnazione dei pin dell'ESP32S3 per il Component Tester.

---

## Pin Occupati

### 1. TFT ST7789 (interfaccia SPI comune, verifica il tuo display.cpp per i valori esatti se diversi)
* **GPIO 5** : CS  (Chip Select)
* **GPIO 16** : DC  (Data/Command)
* **GPIO 17** : RST (Reset)
* **GPIO 18** : SCLK (SPI Clock)
* **GPIO 23** : MOSI (SPI Data Out)
* *(Eventuale pin BLK/LED per retroilluminazione, da verificare se usato via GPIO/PWM)*

### 2. Test Points (TP)
* **GPIO 4** : TP1_PIN
* **GPIO 7** : TP2_PIN (Modificato per risolvere conflitto con TFT CS)
* **GPIO 6** : TP3_PIN

### 3. Funzioni Aggiuntive
* **GPIO 25** : GPIO_STEPUP_EN (Abilitazione convertitore step-up per Zener/LED ad alta tensione)

### 4. Encoder Rotativo (proposti)
* **GPIO 10** : ENCODER_BUTTON_PIN (Pulsante dell'encoder, collegato a GND con pull-up interno)
* **GPIO 11** : ENCODER_PIN_A (Segnale Fase A dell'encoder)
* **GPIO 12** : ENCODER_PIN_B (Segnale Fase B dell'encoder)

---

## Pin Liberi Consigliati (Esempi)
L'ESP32S3 ha molti altri pin GPIO. Alcuni pin generalmente sicuri da usare se non occupati sono: 1, 2, 3, 8, 9, 13, 14, 15, 19, 20, 21, 22, 26, 27, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46.
Assicurati sempre di controllare il datasheet specifico del tuo modulo ESP32S3 per evitare pin con funzioni speciali all'avvio (es. boot pins).
                  ┌────────────────────────┐
                  │                        │
  GND ──┬─────────┤                        ├─────────┬─────── VCC (3.3V)
        │         │   ESP32S3 Modulo       │         │
        │         │                        │         │
        │         └────────────────────────┘         │
        │            ▲          ▲          ▲           │
        │            │          │          │           │
        │            │          │          │           │
        │            │          │          │           │
        │            │          │          │           │
        │            │          │          │           │
        │  ┌─────────┴──────────┴──────────┴─────────┐ │
        │  │                                         │ │
        ├───┤ GPIO 4 (TP1)                            ├───┤ GPIO 5 (TFT CS)
        ├───┤ GPIO 6 (TP3)                            ├───┤ GPIO 16 (TFT DC)
        ├───┤ GPIO 7 (TP2)                            ├───┤ GPIO 17 (TFT RST)
        │   │                                         │   │
        ├───┤ GPIO 10 (Enc Btn)                       ├───┤ GPIO 18 (TFT SCLK)
        ├───┤ GPIO 11 (Enc A)                         ├───┤ GPIO 23 (TFT MOSI)
        ├───┤ GPIO 12 (Enc B)                         │   │
        │   │                                         │   │
        ├───┤ GPIO 25 (StepUp EN)                     │   │
        │   │                                         │   │
        │   │        ... altri GPIO liberi ...        │   │
        │   │                                         │   │
        └─┬─┴─────────────────────────────────────────┴─┬─┘
          │                                           │
          ▼                                           ▼
      A Vari Componenti                           A TFT ST7789

*/