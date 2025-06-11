#ifndef CONFIG_H
#define CONFIG_H

// === Debug ===
#define DEBUG 1
#define VERBOSE 1  // Stampa dettagliata su Serial

// === Pin Mapping ===
#define TP1_PIN 4
#define TP2_PIN 5
#define TP3_PIN 6

// === ADC Settings ===
#define ADC_RESOLUTION 4095.0f  // 12-bit
#define ADC_VREF 3.3f           // Tensione di riferimento

// === Test Thresholds ===
#define VOLTAGE_THRESHOLD_LOW 0.2f
#define VOLTAGE_THRESHOLD_HIGH 3.0f

// === Display Settings ===
#define DISPLAY_WIDTH 240
#define DISPLAY_HEIGHT 320

#endif // CONFIG_H
