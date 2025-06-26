##ifndef CONFIG_H
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
#define TP2_PIN 5  
#define TP3_PIN 6
#define TEST_POINT_1 0
#define TEST_POINT_2 1
#define TEST_POINT_3 2
#define GPIO_STEPUP_EN  38
#define TL431_REF_ADC_PIN 2
#define TP1_470K_PIN  21
#define TP1_680_PIN   33
#define TP2_470K_PIN  34
#define TP2_680_PIN   35
#define TP3_470K_PIN  36
#define TP3_680_PIN   37

// === Pin Encoder / Pulsante ===
#define BUTTON_UP_PIN 15
#define BUTTON_DOWN_PIN 16
#define BUTTON_SELECT_PIN 17

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

Questo documento riassume l'assegnazione aggiornata dei pin dell'ESP32S3 per il Component Tester.

---

## Pin Occupati

### 1. TFT ST7789 (interfaccia SPI)
* **GPIO 10** : CS  (Chip Select)
* **GPIO  9** : DC  (Data/Command)
* **GPIO  8** : RST (Reset)
* **GPIO 12** : SCLK (SPI Clock)
* **GPIO 11** : MOSI (SPI Data Out)
* *(Eventuale pin BLK/LED per retroilluminazione, da verificare se usato via GPIO/PWM)*

### 2. Test Points (TP)
* **GPIO 4**  : TP1_PIN
* **GPIO 5**  : TP2_PIN 
* **GPIO 6**  : TP3_PIN

### 3. Step-Up Enable
* **GPIO 38** : GPIO_STEPUP_EN (abilitazione step-up per Zener/LED alta tensione)

### 4. Resistenze Commutate (gestite via GPIO)
* **GPIO 21** : TP1_470K_PIN
* **GPIO 33** : TP1_680_PIN
* **GPIO 34** : TP2_470K_PIN
* **GPIO 35** : TP2_680_PIN
* **GPIO 36** : TP3_470K_PIN
* **GPIO 37** : TP3_680_PIN

### 5. Riferimento TL431 (lettura ADC)
* **GPIO 2**  : TL431_REF_ADC_PIN

### 6. Encoder Rotativo / Pulsanti
* **GPIO 15** : BUTTON_UP_PIN (Up, collegato a GND con pull-up interno)
* **GPIO 16** : BUTTON_DOWN_PIN (Down, segnale fase A dell'encoder)
* **GPIO 17** : BUTTON_SELECT_PIN (Select, segnale fase B dell'encoder)

---

## Pin Liberi Consigliati (se servono altre espansioni)
L'ESP32S3 WROOM N16R8 ha altri GPIO disponibili: 0, 1, 3, 7, 13, 14, 18, 19, 20, 22, 23, 26, 39, 40, 41, 42, 43, ecc.
Attenzione ai pin riservati per funzioni speciali (boot, QSPI, UART0, ecc) secondo il datasheet.

---
Schema semplificato:

                ┌──────────────────────────────┐
   GND ──┬──────┤        ESP32S3 WROOM         ├───────┬──── VCC (3.3V)
         │      └──────────────────────────────┘       │
         │             ▲   ▲   ▲   ▲   ▲   ▲           │
         │             │   │   │   │   │   │           │
         │             │   │   │   │   │   │           │
         ├── TP1 (4)   │   │   │   │   │   │    TFT_CS (10)
         ├── TP2 (5)   │   │   │   │   │   │    TFT_DC (9)
         ├── TP3 (6)   │   │   │   │   │   │    TFT_RST (8)
         ├── TP1_470K (21)  TP1_680 (33)             TFT_SCLK (12)
         ├── TP2_470K (34)  TP2_680 (35)             TFT_MOSI (11)
         ├── TP3_470K (36)  TP3_680 (37)
         ├── STEPUP_EN (38)
         ├── TL431_REF_ADC (2)
         ├── BUTTON_UP (15)
         ├── BUTTON_DOWN (16)
         └── BUTTON_SELECT (17)

---

*/