/* ************************************************************************
 *
 * ADC functions (Header file for ADC.cpp)
 *
 * (c) 2012-2024 by Markus Reschke (original code declarations)
 * Ported for ESP32S3/Arduino IDE by Gemini AI
 *
 * ************************************************************************ */

#ifndef ADC_H
#define ADC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h> // Per uint8_t, uint16_t, ecc.
#include "config.h" // Per le definizioni dei pin e ADC_DEFAULT_SAMPLES
#include "common.h" // Per eventuali definizioni comuni (es. struct Config)

// --- Dichiarazioni delle funzioni ADC implementate in ADC.cpp ---

/**
 * @brief Initializes the ADC for ESP32S3.
 * Configures resolution and default attenuation.
 */
void initADC(void);

/**
 * @brief Reads a specified ADC channel and returns the voltage in mV.
 * This function adapts the original ATmega logic to ESP32's ADC.
 *
 * @param Channel The logical ADC channel (maps to physical GPIO pin).
 * @return The measured voltage in mV.
 */
uint16_t ReadU(uint8_t Channel);

/**
 * @brief Waits 5ms and then reads the ADC channel.
 * @param Channel The logical ADC channel.
 * @return The measured voltage in mV.
 */
uint16_t ReadU_5ms(uint8_t Channel);

/**
 * @brief Waits 20ms and then reads the ADC channel.
 * @param Channel The logical ADC channel.
 * @return The measured voltage in mV.
 */
uint16_t ReadU_20ms(uint8_t Channel);


#ifdef __cplusplus
}
#endif

#endif /* ADC_H */