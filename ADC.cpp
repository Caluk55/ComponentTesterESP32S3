/* ************************************************************************
 *
 * ADC functions for ESP32S3 with Arduino IDE
 *
 * (c) 2012-2024 by Markus Reschke (original code)
 * Ported for ESP32S3/Arduino IDE by Gemini AI
 *
 * ************************************************************************ */

/*
 * local constants
 */

/* source management */
#define ADC_C // Manteniamo la definizione originale se il progetto la usa per il precompilatore


/*
 * include header files
 */

/* local includes */
#include "config.h"       // Global configuration (dovrà contenere i pin ADC)
#include "common.h"       // Common header file
#include "variables.h"    // Global variables (dove UI, Cfg sono definiti)
#include "functions.h"    // External functions (per waitXms())

// --- Inclusione specifica per Arduino/ESP32 ---
#include <Arduino.h> // Per analogRead(), analogSetAttenuation(), analogReadResolution()

// Dichiara le variabili globali necessarie dal progetto originale, se non già accessibili
// Assicurati che queste siano definite in 'variables.h' o 'config.h' e siano globali
// Ad esempio, la struttura Cfg dovrebbe essere dichiarata altrove (es. variables.h)
// Se Cfg.Vcc e Cfg.Bandgap sono ancora usati per la calibrazione, devono essere inizializzati altrove.
// Per ESP32, useremo preferibilmente la calibrazione interna (analogReadMilliVolts).

// Variabili per la calibrazione dell'ADC dell'ESP32
// La libreria Arduino per ESP32 ha funzioni di calibrazione integrate
// come analogReadMilliVolts(). Non sempre è necessario un "Bandgap" manuale.
// Tuttavia, se il progetto originale si basava su valori di Vcc e Bandgap precisi,
// potremmo volerli mantenere per la compatibilità con le formule di calcolo.
// Per ora, assumiamo che le funzioni di calibrazione ESP32 siano usate primariamente.
// Se hai bisogno di usare Cfg.Vcc o Cfg.Bandgap in futuro, assicurati di calibrarli
// e salvarli correttamente nella flash dell'ESP32 (es. con Preferences o NVS).

// --- Mappatura dei pin ADC (dovrebbero essere definiti in config.h) ---
// Esempio:
// #define TP_REF_PIN     GPIO_NUM_1
// #define TP_ZENER_PIN   GPIO_NUM_2
// ...


/* ************************************************************************
 * ADC Initialization
 * ************************************************************************ */

/**
 * @brief Initializes the ADC for ESP32S3.
 * Configures resolution and default attenuation.
 */
void initADC(void) {
    // Imposta la risoluzione dell'ADC a 12 bit (valori da 0 a 4095)
    // L'ESP32S3 supporta fino a 13 bit, ma 12 bit è un buon compromesso e compatibile
    // con la risoluzione "1024" (10 bit) del 328, permettendo più dettaglio.
    analogReadResolution(12);

    // Imposta l'attenuazione predefinita per l'ADC.
    // L'attenuazione determina il range di tensione in ingresso.
    // ADC_11db = ~0-3.9V, è la più comune per letture ampie.
    // Altre opzioni: ADC_0db (~0-1.1V), ADC_2_5db (~0-1.5V), ADC_6db (~0-2.2V)
    // Iniziamo con l'attenuazione più alta per la massima flessibilità.
    analogSetAttenuation(ADC_11db);

    // Nota: Non c'è una "dummy conversion" esplicita necessaria come sull'ATmega.
    // Le prime letture potrebbero essere meno stabili, ma la libreria gestisce
    // questo implicitamente.

    Serial.println("ADC initialized for ESP32S3.");
}
/* ************************************************************************
 * Read ADC channel and return voltage in mV for ESP32S3
 * ************************************************************************ */

/**
 * @brief Reads a specified ADC channel and returns the voltage in mV.
 * This function adapts the original ATmega logic to ESP32's ADC.
 *
 * @param Channel The logical ADC channel (maps to physical GPIO pin).
 * @return The measured voltage in mV.
 */
uint16_t ReadU(uint8_t Channel)
{
    uint32_t rawValue = 0;
    uint16_t voltage_mV = 0;
    uint8_t currentPin = 0; // Il pin GPIO fisico da leggere

    // Mappa il "Channel" logico (0-2 o specifici pin) ai pin GPIO fisici dell'ESP32S3
    // Assicurati che i tuoi #define in config.h siano corretti per questi pin.
    // Useremo i pin definiti nel config.h per questo mapping.
    // Questa parte deve essere robusta e coprire tutti i canali ADC che intendi usare.
    switch (Channel) {
        case TP_REF_ADC_CHANNEL:
            currentPin = TP_REF_PIN;
            break;
        case TP_ZENER_ADC_CHANNEL:
            currentPin = TP_ZENER_PIN;
            break;
        case TP_CAP_ADC_CHANNEL:
            currentPin = TP_CAP_PIN;
            break;
        case BAT_MONITOR_ADC_CHANNEL:
            currentPin = BAT_MONITOR_PIN;
            break;
        case TP_LOGIC_ADC_CHANNEL:
            currentPin = TP_LOGIC_PIN;
            break;
        // Aggiungi altri case per eventuali altri canali ADC necessari
        default:
            // Gestione di un canale non valido (potrebbe essere un errore o un pin non mappato)
            Serial.print("ERROR: Invalid ADC Channel specified: ");
            Serial.println(Channel);
            return 0; // Ritorna 0 o un valore di errore
    }

    // --- Gestione dinamica dell'attenuazione (Auto-scaling) ---
    // Simula la logica di autoscaling dell'ATmega328, ma usando le attenuazioni dell'ESP32.
    // L'ESP32 non ha un riferimento "bandgap" separato nello stesso senso, ma diverse attenuazioni.
    // Il riferimento interno è sempre 1.1V, ma l'attenuazione scala l'input prima del convertitore.

    // Tentiamo prima con l'attenuazione più sensibile (ADC_0db, ~0-1.1V)
    // Se la lettura è al massimo o vicina, cambiamo ad attenuazione più alta.
    // Questo replica la logica di "switch a bandgap" per basse tensioni.
    analogSetAttenuation(ADC_0db); // ~0-1.1V range
    // Eseguiamo un paio di letture iniziali per stabilizzare
    analogRead(currentPin);
    delayMicroseconds(10); // Piccola attesa per stabilizzazione

    rawValue = analogRead(currentPin);

    // Se la lettura è troppo alta per l'attenuazione 0db, passiamo a una maggiore.
    // Una lettura di 4095 (per 12 bit) è il massimo.
    // Usiamo una soglia (es. 90% del max) per decidere se cambiare attenuazione.
    if (rawValue > (4095 * 0.9)) { // Se più del 90% del range di 0db è raggiunto
        analogSetAttenuation(ADC_6db); // Passa a ~0-2.2V
        delayMicroseconds(10);
        rawValue = analogRead(currentPin); // Rileggiamo con la nuova attenuazione
    }
    if (rawValue > (4095 * 0.9)) { // Se ancora troppo alto per 6db
        analogSetAttenuation(ADC_11db); // Passa a ~0-3.9V (o max tensione supportata)
        delayMicroseconds(10);
        rawValue = analogRead(currentPin); // Rileggiamo
    }
    // Puoi aggiungere un ulteriore check se la tensione supera anche i 3.9V del 11db,
    // ma l'ESP32 non dovrebbe mai ricevere più di 3.3V sui pin GPIO,
    // a meno che tu non abbia un partitore di tensione esterno per misurare tensioni più alte.

    // --- Campionamento e Media ---
    // Come l'originale, facciamo multiple letture e ne calcoliamo la media.
    // Cfg.Samples dovrebbe essere definito in config.h (es. 25)
    uint32_t sumRawValue = 0;
    for (uint8_t i = 0; i < Cfg.Samples; i++) {
        sumRawValue += analogRead(currentPin);
        // Possibile delayMicroseconds(10) o analogRead(currentPin) in più per stabilizzare
    }
    rawValue = sumRawValue / Cfg.Samples;


    // --- Conversione a mV usando la calibrazione interna dell'ESP32 ---
    // analogReadMilliVolts() è la funzione più semplice e affidabile
    // per ottenere la tensione in mV, poiché usa la calibrazione e l'attenuazione correnti.
    voltage_mV = analogReadMilliVolts(currentPin);

    // Se hai bisogno di usare Cfg.Vcc o Cfg.Bandgap per la tua logica di calibrazione
    // specifica (come nel progetto originale), dovrai considerare come questi
    // valori vengono misurati e aggiornati sull'ESP32.
    // Per esempio, potresti leggere la tensione del 3.3V dell'ESP32 (se hai un pin dedicato)
    // e memorizzarla in Cfg.Vcc per un scaling relativo.
    // Per ora, ci affidiamo a analogReadMilliVolts per la precisione diretta.

    return voltage_mV;
}
/* ************************************************************************
 * convenience functions
 * ************************************************************************ */

/**
 * @brief Waits 5ms and then reads the ADC channel.
 * @param Channel The logical ADC channel.
 * @return The measured voltage in mV.
 */
uint16_t ReadU_5ms(uint8_t Channel)
{
    delay(5); // Arduino delay() per millisecondi
    return ReadU(Channel);
}

/**
 * @brief Waits 20ms and then reads the ADC channel.
 * @param Channel The logical ADC channel.
 * @return The measured voltage in mV.
 */
uint16_t ReadU_20ms(uint8_t Channel)
{
    delay(20); // Arduino delay() per millisecondi
    return ReadU(Channel);
}


/* ************************************************************************
 * clean-up of local constants
 * ************************************************************************ */

/* source management */
#undef ADC_C


/* ************************************************************************
 * EOF
 * ************************************************************************ */