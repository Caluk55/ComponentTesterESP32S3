/* ************************************************************************
 *
 * global configuration, setup and settings
 *
 * (c) 2012-2024 by Markus Reschke
 * based on code from Markus Frejek and Karl-Heinz Kübbeler
 *
 * Adapted for ESP32S3 with ST7789 TFT display using TFTeSPI library
 * GPIO pins chosen for ESP32S3 (assuming TFT pins are handled by User_Setup.h)
 *
 * ************************************************************************ */


/* source management */
#define CONFIG_H


/*
 * Original MCU specific settings (port and pin assignments) and display
 * settings are no longer relevant for ESP32S3.
 * They were handled by config_<MCU>.h files (e.g., ATmega328: config_328.h).
 * Pin assignments for ESP32S3 are defined directly in this file
 * (excluding TFT pins which are in TFTeSPI User_Setup.h).
 */


/* ************************************************************************
 * Hardware options
 * ************************************************************************ */

// --- Display Configuration ---
// These values are for a 240x320 ST7789 display
#define DISPLAY_WIDTH_PIXELS  240
#define DISPLAY_HEIGHT_PIXELS 320

// Font dimensions in pixels. Adjust these based on the font you select in display.cpp
// For example, if using a 6x8 monospace font (common for character-based UIs):
#define FONT_CHAR_WIDTH  6
#define FONT_CHAR_HEIGHT 8

// Character grid dimensions based on display and font.
// These are used by the original UI logic.
#define UI_CharMax_X (DISPLAY_WIDTH_PIXELS / FONT_CHAR_WIDTH)
#define UI_CharMax_Y (DISPLAY_HEIGHT_PIXELS / FONT_CHAR_HEIGHT)
// === INIZIO DELLE MODIFICHE/AGGIUNTE ===
// --- DEFINIZIONE DEI PIN GPIO FISICI SULLA TUA ESP32S3 ---
// Assegna un NOME (es. TP1_GPIO) a un PIN FISICO (es. GPIO_NUM_1).
// I commenti indicano il CANALE ADC associato a quel GPIO specifico.
//
// Verifica attentamente che questi GPIO non siano già usati per il display TFT
// o altre periferiche cruciali nel tuo 'User_Setup.h' di TFTeSPI!
//
#define TP1_GPIO            GPIO_NUM_1      // Corrisponde a ADC1_CH0
#define TP2_GPIO            GPIO_NUM_2      // Corrisponde a ADC1_CH1
#define TP3_GPIO            GPIO_NUM_3      // Corrisponde a ADC1_CH2

#define BAT_MONITOR_GPIO    GPIO_NUM_4      // Corrisponde a ADC1_CH3

// Se hai attivato HW_LOGIC_PROBE nel config.h, userai anche questo pin:
// #define TP_LOGIC_GPIO       GPIO_NUM_47     // Corrisponde a ADC2_CH6


// --- MAPPATURA DEI CANALI LOGICI DEL PROGETTO ORIGINALE AI PIN FISICI SCELTI ---
// Qui le funzioni del codice legacy (che si aspettano un "canale" come TP_REF_ADC_CHANNEL)
// vengono re-indirizzate al PIN FISICO che abbiamo scelto per quella misurazione.
//
// Esempio: La funzione 'ReadU(TP_REF_ADC_CHANNEL)' ora leggerà dal GPIO_NUM_1 (TP1_GPIO).
//
#define TP_REF_ADC_CHANNEL      TP1_GPIO
#define TP_ZENER_ADC_CHANNEL    TP2_GPIO        // Se il test Zener usa TP2
#define TP_CAP_ADC_CHANNEL      TP3_GPIO        // Se il test Capacità usa TP3
#define BAT_MONITOR_ADC_CHANNEL BAT_MONITOR_GPIO // Il monitor batteria usa il pin dedicato

// Se HW_LOGIC_PROBE è attivo, mappa anche questo:
// #define TP_LOGIC_ADC_CHANNEL TP_LOGIC_GPIO


// --- IMPOSTAZIONE DEL NUMERO DI CAMPIONI ADC ---
// Se non è già definito, aggiungi questa riga.
// Questo valore è usato nella funzione ReadU in ADC.cpp.
#ifndef ADC_DEFAULT_SAMPLES
#define ADC_DEFAULT_SAMPLES 25
#endif
// === FINE DELLE MODIFICHE/AGGIUNTE ===


// --- User Interface Input (Rotary Encoder / Buttons) ---
/*
 * rotary encoder for user interface
 * - uncomment to enable and set ENCODER_PULSES & ENCODER_STEPS
 * - Chosen ESP32S3 GPIO pins: GPIO4, GPIO5 (for A/B), GPIO6 (for button)
 */
//#define HW_ENCODER
#ifdef HW_ENCODER
    #define ENCODER_PIN_A       GPIO_NUM_4
    #define ENCODER_PIN_B       GPIO_NUM_5
    #define ENCODER_BUTTON_PIN  GPIO_NUM_6
#endif

/*
 * Number of Gray code pulses per step or detent for the rotary encoder
 */
#define ENCODER_PULSES    4

/*
 * Number of detents or steps
 */
#define ENCODER_STEPS     24

/*
 * increase/decrease push buttons for user interface
 * - alternative for rotary encoder
 * - uncomment to enable
 * - Chosen ESP32S3 GPIO pins: GPIO7 (INC), GPIO8 (DEC)
 */
//#define HW_INCDEC_KEYS
#ifdef HW_INCDEC_KEYS
    #define KEY_INC_PIN     GPIO_NUM_7
    #define KEY_DEC_PIN     GPIO_NUM_8
#endif


/*
 * 2.5V voltage reference for Vcc check (optional for ESP32S3)
 * - uncomment to enable and adjust UREF_25 below
 * - Chosen ESP32S3 GPIO pin: GPIO1 (ADC input)
 */
//#define HW_REF25
#define UREF_25           2495
#ifdef HW_REF25
    #define TP_REF_PIN      GPIO_NUM_1   // ADC1_CH0
#endif


/*
 * Probe protection relay for discharging caps (optional)
 * - uncomment to enable
 * - Chosen ESP32S3 GPIO pin: GPIO10
 */
//#define HW_DISCHARGE_RELAY
#ifdef HW_DISCHARGE_RELAY
    #define RELAY_PIN       GPIO_NUM_10
#endif


/*
 * Zener check / voltage measurement up to 50V DC (optional)
 * - uncomment to enable
 * - Chosen ESP32S3 GPIO pin: GPIO2 (ADC input)
 */
//#define HW_ZENER
#ifdef HW_ZENER
    #define TP_ZENER_PIN    GPIO_NUM_2   // ADC1_CH1
#endif

/*
 * non-standard voltage divider for Zener check
 * - uncomment to enable and adjust resistor values
 */
//#define ZENER_DIVIDER_CUSTOM
#define ZENER_R1          180000
#define ZENER_R2          20000

/*
 * alternative mode for Zener check: don't switch boost converter
 * - uncomment to enable
 */
//#define ZENER_UNSWITCHED

/*
 * alternative mode for Zener check: switch converter via dedicated MCU pin
 * - uncomment to enable and choose one drive method
 * - Chosen ESP32S3 GPIO pin: GPIO11
 */
//#define ZENER_SWITCHED
//#define ZENER_BOOST_HIGH          /* high active */
#define ZENER_BOOST_LOW             /* low active */
#ifdef ZENER_SWITCHED
    #define BOOST_CTRL_PIN  GPIO_NUM_11
#endif


/*
 * Zener check during normal probing
 * - requires boost converter running all the time (ZENER_UNSWITCHED)
 * - uncomment to enable
 */
//#define HW_PROBE_ZENER
#define ZENER_VOLTAGE_MIN     1000      /* min. voltage in mV */
#define ZENER_VOLTAGE_MAX     30000     /* max. voltage in mV */


/*
 * fixed signal output (e.g., from a DAC or specific PWM channel)
 * - uncomment to enable
 * - You'll need to define the specific pin if this is used, e.g., GPIO12
 */
//#define HW_FIXED_SIGNAL_OUTPUT
#ifdef HW_FIXED_SIGNAL_OUTPUT
    #define FIXED_SIGNAL_OUT_PIN GPIO_NUM_12
#endif


/*
 * basic frequency counter
 * - uncomment to enable
 * - Chosen ESP32S3 GPIO pin: GPIO13 (can be configured for pulse counting)
 */
//#define HW_FREQ_COUNTER_BASIC
#ifdef HW_FREQ_COUNTER_BASIC
    #define FREQ_IN_PIN     GPIO_NUM_13
#endif

/*
 * extended frequency counter
 * - uncomment to enable
 * - Chosen ESP32S3 GPIO pins: GPIO14, GPIO15
 */
//#define HW_FREQ_COUNTER_EXT
#define FREQ_COUNTER_PRESCALER    16   /* 16:1 */
//#define FREQ_COUNTER_PRESCALER    32   /* 32:1 */
#ifdef HW_FREQ_COUNTER_EXT
    #define COUNTER_CTRL_PIN_1  GPIO_NUM_14
    #define COUNTER_CTRL_PIN_2  GPIO_NUM_15
#endif


/*
 * ring tester (LOPT/FBT tester)
 * - uncomment to enable
 * - Chosen ESP32S3 GPIO pin: GPIO16
 */
//#define HW_RING_TESTER
#define RING_TESTER_PIN             /* dedicated pin */
//#define RING_TESTER_PROBES          /* probes */
#ifdef HW_RING_TESTER
    #ifdef RING_TESTER_PIN
        #define RINGTESTER_OUT_PIN  GPIO_NUM_16
    #endif
#endif


/*
 * event counter
 * - uncomment to enable
 * - Chosen ESP32S3 GPIO pin: GPIO17
 */
//#define HW_EVENT_COUNTER
#ifdef HW_EVENT_COUNTER
    #define EVENT_IN_PIN    GPIO_NUM_17
#endif

/*
 * trigger output for event counter
 * - uncomment to enable
 */
//#define EVENT_COUNTER_TRIGGER_OUT


/*
 * IR remote control detection/decoder (via dedicated MCU pin)
 * - uncomment to enable
 * - Chosen ESP32S3 GPIO pin: GPIO18
 */
//#define HW_IR_RECEIVER
#ifdef HW_IR_RECEIVER
    #define IR_RX_PIN       GPIO_NUM_18
#endif


/*
 * fixed cap for self-adjustment
 * - uncomment to enable
 * - Chosen ESP32S3 GPIO pins: GPIO3 (ADC input), GPIO19 (control)
 */
//#define HW_ADJUST_CAP
#ifdef HW_ADJUST_CAP
    #define TP_CAP_PIN      GPIO_NUM_3   // ADC1_CH2
    #define ADJUST_CTRL_PIN GPIO_NUM_19
#endif


/*
 * L/C meter hardware option
 * - uncomment to enable
 * - Chosen ESP32S3 GPIO pins: GPIO20, GPIO21
 */
//#define HW_LC_METER
#define LC_METER_C_REF        10000
//#define LC_METER_SHOW_FREQ
#ifdef HW_LC_METER
    #define LC_CTRL_PIN_1   GPIO_NUM_20
    #define LC_CTRL_PIN_2   GPIO_NUM_21
#endif


/*
 * relay for parallel cap (sampling ADC)
 * - uncomment to enable (not implemented yet)
 * - Chosen ESP32S3 GPIO pin: GPIO22
 */
//#define HW_CAP_RELAY
#ifdef HW_CAP_RELAY
    #define CAP_RELAY_PIN   GPIO_NUM_22
#endif


/*
 * Logic Probe
 * - uncomment to enable and adjust resistor values
 * - Chosen ESP32S3 GPIO pin: GPIO47 (ADC input, can be ADC2_CH6)
 */
//#define HW_LOGIC_PROBE
#define LOGIC_PROBE_R1        10000
#define LOGIC_PROBE_R2        3300
#ifdef HW_LOGIC_PROBE
    #define TP_LOGIC_PIN    GPIO_NUM_47  // ADC2_CH6
#endif


/*
 * Buzzer
 * - uncomment to enable and also select the correct buzzer type
 * - Chosen ESP32S3 GPIO pin: GPIO48 (can be used with LEDC for passive buzzer tones)
 */
//#define HW_BUZZER
#define BUZZER_ACTIVE               /* active buzzer (simple on/off) */
//#define BUZZER_PASSIVE              /* passive buzzer (needs PWM for tones) */
#ifdef HW_BUZZER
    #define BUZZER_PIN      GPIO_NUM_48
#endif


/*
 * MAX6675 thermocouple converter
 * - uncomment to enable
 * - ESP32S3 uses hardware SPI, define CS pin below (SCK, MISO, MOSI usually from TFT SPI)
 * - Chosen ESP32S3 GPIO pin: GPIO38 (CS)
 */
//#define HW_MAX6675
#ifdef HW_MAX6675
    #define MAX6675_CS      GPIO_NUM_38  // Chip Select
#endif


/*
 * MAX31855 thermocouple converter
 * - uncomment to enable
 * - ESP32S3 uses hardware SPI, define CS pin below
 * - Chosen ESP32S3 GPIO pin: GPIO39 (CS)
 */
//#define HW_MAX31855
#ifdef HW_MAX31855
    #define MAX31855_CS     GPIO_NUM_39  // Chip Select
#endif


/*
 * flashlight / general purpose switched output
 * - uncomment to enable
 * - Chosen ESP32S3 GPIO pin: GPIO40
 */
//#define HW_FLASHLIGHT
#ifdef HW_FLASHLIGHT
    #define FLASHLIGHT_CTRL GPIO_NUM_40
#endif


/*
 * BH1750VFI ambient light sensor
 * - requires I2C bus (ESP32S3 has hardware I2C). SDA/SCL usually defined globally.
 * - uncomment to enable and also select the correct I2C address
 */
//#define HW_BH1750
#define BH1750_I2C_ADDR       0x23      /* I2C address 0x23 (ADDR low) */
//#define BH1750_I2C_ADDR       0x5c       /* I2C address 0x5c (ADDR high) */


/* ************************************************************************
 * software options
 * ************************************************************************ */


/*
 * PWM generator with simple user interface
 * - uncomment to enable
 */
#define SW_PWM_SIMPLE


/*
 * PWM generator with fancy user interface
 * - uncomment to enable
 */
//#define SW_PWM_PLUS

/*
 * PWM generator: show also pulse duration
 * - uncomment to enable
 */
//#define PWM_SHOW_DURATION


/*
 * Inductance measurement
 * - uncomment to enable
 */
#define SW_INDUCTOR


/*
 * ESR measurement
 * - uncomment to enable
 */
#define SW_ESR
//#define SW_OLD_ESR


/*
 * ESR Tool (in-circuit ESR measurement)
 * - uncomment to enable
 */
//#define SW_ESR_TOOL


/*
 * check for rotary encoders
 * - uncomment to enable
 */
//#define SW_ENCODER


/*
 * squarewave signal generator
 * - uncomment to enable
 */
#define SW_SQUAREWAVE


/*
 * IR remote control detection/decoder (via probes)
 * - uncomment to enable
 */
#define SW_IR_RECEIVER


/*
 * probe pinout for IR receiver module
 * - select one
 */
#define SW_IR_RX_PINOUT_G_V_D       /* 1-Gnd 2-Vcc 3-Data (default) */
//#define SW_IR_RX_PINOUT_D_G_V       /* 1-Data 2-Gnd 3-Vcc */
//#define SW_IR_RX_PINOUT_D_V_G       /* 1-Data 2-Vcc 3-Gnd */


/*
 * current limiting resistor for IR receiver module
 * - uncomment to disable resistor
 */
//#define SW_IR_DISABLE_RESISTOR


/*
 * confirmation beep for valid data frame/packet
 * - uncomment to enable
 */
//#define SW_IR_RX_BEEP


/*
 * additional protocols for IR remote control detection/decoder
 * - uncomment to enable
 */
//#define SW_IR_RX_EXTRA


/*
 * IR remote control sender
 * - uncomment to enable
 */
//#define SW_IR_TRANSMITTER


/*
 * Alternative delay loop for IR remote control sender
 * - uncomment to enable
 */
//#define SW_IR_TX_ALTDELAY


/*
 * additional protocols for IR remote control sender
 * - uncomment to enable
 */
//#define SW_IR_TX_EXTRA


/*
 * check for opto couplers
 * - uncomment to enable
 */
#define SW_OPTO_COUPLER


/*
 * check for Unijunction Transistor
 * - uncomment to enable
 */
#define SW_UJT


/*
 * check for Schottky Transistor (Schottky-clamped BJT)
 * - uncomment to enable
 */
#define SW_SCHOTTKY_BJT


/*
 * Servo Check
 * - uncomment to enable
 */
//#define SW_SERVO


/*
 * DS18B20 - OneWire temperature sensor
 * - uncomment to enable
 */
//#define SW_DS18B20


/*
 * DS18S20 - OneWire temperature sensor
 * - uncomment to enable
 */
//#define SW_DS18S20
//#define DS18S20_HIGHRES         /* high resolution (0.01°C) */


/*
 * OneWire: read and display ROM code
 * - uncomment to enable
 */
//#define ONEWIRE_READ_ROM


/*
 * scan OneWire bus for devices and list their ROM codes
 * - uncomment to enable
 */
//#define SW_ONEWIRE_SCAN


/*
 * capacitor leakage check
 * - uncomment to enable
 */
//#define SW_CAP_LEAKAGE


/*
 * display reverse hFE for BJTs
 * - uncomment to enable
 */
//#define SW_REVERSE_HFE


/*
 * display I_C/I_E test current for hFE measurement
 * - uncomment to enable
 */
//#define SW_HFE_CURRENT


/*
 * R/C/L monitors
 * - uncomment to enable (one or more)
 */
//#define SW_MONITOR_R          /* just R */
//#define SW_MONITOR_C          /* just C plus ESR */
//#define SW_MONITOR_L          /* just L */
//#define SW_MONITOR_RCL        /* R plus L, or C plus ESR */
//#define SW_MONITOR_RL         /* R plus L */


/*
 * C/L monitors: auto hold
 * - uncomment to enable (one or more)
 */
//#define SW_MONITOR_HOLD_ESR   /* auto-hold ESR (C monitor) */
//#define SW_MONITOR_HOLD_L     /* auto-hold L (L monitor) */


/*
 * DHT11, DHT22 and compatible humidity & temperature sensors
 * - uncomment to enable
 */
//#define SW_DHTXX


/*
 * check resistor for matching E series norm value
 * - uncomment to enable (one or more)
 */
//#define SW_R_E24_5_T          /* E24 5% tolerance, text */
//#define SW_R_E24_5_CC         /* E24 5% tolerance, color-code */
//#define SW_R_E24_1_T          /* E24 1% tolerance, text */
//#define SW_R_E24_1_CC         /* E24 1% tolerance, color-code */
//#define SW_R_E96_T            /* E96 1% tolerance, text */
//#define SW_R_E96_CC           /* E96 1% tolerance, color-code */
//#define SW_R_E96_EIA96        /* E96 1% tolerance, EIA-96 code */


/*
 * check capacitor for matching E series norm value
 * - uncomment to enable (one or more)
 */
//#define SW_C_E6_T             /* E6 20% tolerance, text */
//#define SW_C_E12_T            /* E12 10% tolerance, text */


/*
 * check inductor for matching E series norm value
 * - uncomment to enable (one or more)
 */
//#define SW_L_E6_T             /* E6 20% tolerance, text */
//#define SW_L_E12_T            /* E12 10% tolerance, text */


/*
 * continuity check
 * - uncomment to enable
 */
//#define SW_CONTINUITY_CHECK


/*
 * show additional info for a possible potentiometer/trimpot
 * - uncomment to enable
 */
//#define SW_R_TRIMMER


/*
 * show self-discharge voltage loss (in %) of a capacitor > 50nF
 * - uncomment to enable
 */
//#define SW_C_VLOSS


/*
 * photodiode check
 * - uncomment to enable
 */
//#define SW_PHOTODIODE


/*
 * diode/LED quick-check
 * - uncomment to enable
 */
//#define SW_DIODE_LED



/* ************************************************************************
 * workarounds for some testers
 * ************************************************************************ */


/*
 * Disable hFE measurement with common collector circuit and Rl as
 * base resistor.
 * - uncomment to enable
 */
//#define NO_HFE_C_RL


/*
 * Alternative power control for clones with SCT15L104W management MCU.
 * - uncomment to enable
 */
//#define PASSIVE_POWER_CTRL



/* ************************************************************************
 * workarounds for some IDEs
 * ************************************************************************ */


/*
 * Oscillator startup cycles (after wakeup from power-safe mode):
 * - Keep as is for now, generally not an issue for ESP32
 */
#ifndef OSC_STARTUP
  #define OSC_STARTUP    16384
#endif



/* ************************************************************************
 * user interface
 * ************************************************************************ */


/*
 * Language of user interface.
 * - Uncomment UI_ITALIAN
 */
//#define UI_ENGLISH
//#define UI_BRAZILIAN
//#define UI_CZECH
//#define UI_CZECH_2
//#define UI_DANISH
//#define UI_FRENCH
//#define UI_GERMAN
#define UI_ITALIAN  // <--- ENABLED FOR ITALIAN LANGUAGE
//#define UI_POLISH
//#define UI_POLISH_2
//#define UI_ROMANIAN
//#define UI_RUSSIAN
//#define UI_RUSSIAN_2
//#define UI_SPANISH


/*
 * Use comma instead of dot to indicate a decimal fraction.
 * - uncomment to enable
 */
//#define UI_COMMA


/*
 * Display temperatures in Fahrenheit instead of Celsius.
 * - uncomment to enable
 */
//#define UI_FAHRENHEIT


/*
 * Display hexadecimal values in uppercase instead of lowercase
 * - uncomment to enable
 */
//#define UI_HEX_UPPERCASE


/*
 * Set the default operation mode to auto-hold.
 * - uncomment to enable
 */
//#define UI_AUTOHOLD


/*
 * Switch temporarily to auto-hold mode when a component is detected.
 * - uncomment to enable
 */
//#define UI_AUTOHOLD_FOUND


/*
 * Trigger the menu also by a short circuit of all three probes.
 * - uncomment to enable
 */
//#define UI_SHORT_CIRCUIT_MENU


/*
 * Show key hints instead of cursor if available.
 * - uncomment to enable
 */
//#define UI_KEY_HINTS


/*
 * Enter menu to select adjustment profile after powering on.
 * - uncomment to enable
 */
//#define UI_CHOOSE_PROFILE


/*
 * Add a third profile for adjustment values.
 * - uncomment to enable
 */
//#define UI_THREE_PROFILES


/*
 * Output components found also via TTL serial interface.
 * - uncomment to enable
 * - SERIAL_BITBANG or SERIAL_HARDWARE (will use ESP32's UART, TX pin e.g. GPIO43)
 */
//#define UI_SERIAL_COPY
#ifdef UI_SERIAL_COPY
    #define SERIAL_TX_PIN   GPIO_NUM_43
#endif


/*
 * Control tester via TTL serial interface.
 * - uncomment to enable
 * - RX pin e.g. GPIO44
 */
//#define UI_SERIAL_COMMANDS
#ifdef UI_SERIAL_COMMANDS
    #define SERIAL_RX_PIN   GPIO_NUM_44
#endif


/*
 * Maximum time to wait after probing (in ms).
 */
#define CYCLE_DELAY     3000


/*
 * Maximum number of probing runs without any component found in a row.
 */
#define CYCLE_MAX       5


/*
 * Automatic power-off when no button is pressed for a while (in s).
 * - uncomment to enable, also adjust timeout (in s)
 */
//#define POWER_OFF_TIMEOUT     60


/*
 * fancy pinout with component symbols for 3-pin semiconductors
 * - requires graphics display and symbol bitmaps (handled by display.cpp and icon arrays)
 * - uncomment to enable
 */
#define SW_SYMBOLS


/*
 * fancy pinout: show right-hand probe numbers above/below symbol
 * - uncomment to enable
 */
//#define UI_PINOUT_ALT


/*
 * failed test run: display question mark symbol
 * - uncomment to enable
 */
//#define UI_QUESTION_MARK


/*
 * any Zener check: display Zener diode symbol
 * - uncomment to enable
 */
//#define UI_ZENER_DIODE


/*
 * extended frequency counter: display quartz crystal symbol for LF/HF modes
 * - uncomment to enable
 */
//#define UI_QUARTZ_CRYSTAL


/*
 * DS18B20/DS18S20/DHTXX: display sensor symbol
 * - uncomment to enable
 */
//#define UI_ONEWIRE


/*
 * disable text based pinout for 3-pin semiconductors
 * - uncomment to enable
 */
//#define UI_NO_TEXTPINOUT


/*
 * disable text based pinout of body/intrinsic diode for MOSFETs
 * - uncomment to enable
 */
//#define UI_NO_BODYDIODE_TEXTPINOUT


/*
 * battery status: display icon
 * - uncomment to enable
 */
//#define UI_BATTERY


/*
 * battery status: display in last line after showing probing result
 * - uncomment to enable
 */
//#define UI_BATTERY_LASTLINE


/*
 * display probe IDs using reversed colors
 * - uncomment to enable
 */
//#define UI_PROBE_REVERSED


/*
 * color coding for probes
 * - requires color graphics display (TFTeSPI handles this)
 * - uncomment to enable
 * - Colors defined below (COLOR_PROBE_1, COLOR_PROBE_2, COLOR_PROBE_3)
 */
#define UI_PROBE_COLORS


/*
 * colored titles
 * - requires color graphics display
 * - uncomment to enable
 * - Color defined below (COLOR_TITLE)
 */
//#define UI_COLORED_TITLES


/*
 * colored cursor and key hints
 * - requires color graphics display
 * - uncomment to enable
 * - Color defined below (COLOR_CURSOR)
 */
//#define UI_COLORED_CURSOR


/*
 * colored values
 * - requires color graphics display
 * - uncomment to enable
 * - Color defined below (COLOR_VALUE)
 */
//#define UI_COLORED_VALUES


/*
 * menues: scroll menu page-wise instead of item-wise
 * - uncomment to enable
 */
//#define UI_MENU_PAGEMODE


/*
 * automatically exit main menu after running function/tool
 * - uncomment to enable
 */
//#define UI_MAINMENU_AUTOEXIT


/*
 * main menu: power off tester
 * - uncomment to enable
 */
//#define SW_POWER_OFF


/*
 * main menu: display font for test purposes
 * - uncomment to enable
 */
//#define SW_FONT_TEST
//#define FONT_PACKED           /* packed output format */


/*
 * main menu: display component symbols for test purposes
 * - uncomment to enable
 */
//#define SW_SYMBOL_TEST


/*
 * Round some values if appropriate.
 * - uncomment to enable
 */
//#define UI_ROUND_DS18B20


/*
 * Center-align infos and some other texts
 * - uncomment to enable
 */
//#define UI_CENTER_ALIGN


/*
 * confirmation beep when probing is done
 * - uncomment to enable
 */
//#define UI_PROBING_DONE_BEEP


/*
 * storage of firmware data (texts, tables etc)
 * - DATA_FLASH is generally preferred for ESP32.
 * - Adjustment data will likely use ESP32's NVS (Non-Volatile Storage).
 * - Uncomment DATA_FLASH if preferred, otherwise DATA_EEPROM (will require NVS implementation)
 */
//#define DATA_EEPROM           /* store data in EEPROM */
#define DATA_FLASH            /* store data in Flash */



/* ************************************************************************
 * power management
 * ************************************************************************ */


/*
 * type of power switch
 * - soft-latching power switch (default)
 * - manual power switch
 * - enable one
 */
#define POWER_SWITCH_SOFT
//#define POWER_SWITCH_MANUAL


/*
 * Battery monitoring mode:
 * - BAT_NONE      disable battery monitoring completely
 * - BAT_DIRECT    direct measurement of battery voltage (< 5V)
 * - BAT_DIVIDER   measurement via voltage divider
 * - uncomment one of the modes
 * - Chosen ESP32S3 GPIO pin: GPIO4 (ADC input, ADC1_CH3)
 */
//#define BAT_NONE
//#define BAT_DIRECT
#define BAT_DIVIDER
#ifdef BAT_DIVIDER
    #define BAT_MONITOR_PIN GPIO_NUM_4   // ADC1_CH3 (Good for battery input)
#endif


/*
 * Unmonitored optional external power supply
 * - uncomment to enable
 */
//#define BAT_EXT_UNMONITORED


/*
 * Voltage divider for battery monitoring
 * - BAT_R1: top resistor in Ohms
 * - BAT_R2: bottom resistor in Ohms
 */
#define BAT_R1            10000
#define BAT_R2            3300


/*
 * Voltage drop by reverse voltage protection diode and power management
 * transistor (in mV). Measure this value in your circuit.
 */
#define BAT_OFFSET        290


/*
 * Battery weak voltage (in mV).
 */
#define BAT_WEAK          7400


/*
 * Battery low voltage (in mV).
 */
#define BAT_LOW           6400


/*
 * Enter sleep mode when idle to save power.
 * - uncomment to enable
 */
#define SAVE_POWER



/* ************************************************************************
 * measurement settings and offsets
 * ************************************************************************ */

/*
 * ADC voltage reference (in mV).
 * For ESP32S3, the default internal ADC reference is 1100mV (1.1V).
 * This value is critical for accurate measurements.
 * If you use a VCC reference (e.g., 3.3V), adjust accordingly.
 * For general purpose, 1100mV is a safe starting point for internal reference.
 */
#define UREF_VCC          1100  // <--- Adjusted for ESP32S3 typical internal ADC reference


/*
 * Offset for the internal bandgap voltage reference (in mV): -100 up to 100
 * - This may need specific calibration for ESP32 ADC.
 */
#define UREF_OFFSET      0


/*
 * Exact values of probe resistors.
 */
/* Rl in Ohms */
#define R_LOW             680

/* Rh in Ohms */
#define R_HIGH            470000


/*
 * Offset for systematic error of resistor measurement with Rh (470k)
 * in Ohms.
 */
#define RH_OFFSET         350


/*
 * Resistance of probes (in 0.01 Ohms).
 */
#define R_ZERO            20


/*
 * Use probe pair specific resistance offsets instead of an
 * average value for all probes.
 * - uncomment to enable
 */
//#define R_MULTIOFFSET


/*
 * Capacitance of probes (in pF).
 */
#define C_ZERO            43


/*
 * Use probe pair specific capacitance offsets instead of an
 * average value for all probes.
 * - uncomment to enable
 */
//#define CAP_MULTIOFFSET


/*
 * Maximum voltage at which we consider a capacitor being
 * discharged (in mV).
 */
#define CAP_DISCHARGED    2


/*
 * Correction factors for capacitors (in 0.1%)
 */
#define CAP_FACTOR_SMALL      0      /* no correction */
#define CAP_FACTOR_MID        -40    /* -4.0% */
#define CAP_FACTOR_LARGE      -90    /* -9.0% */


/*
 * Number of ADC samples to perform for each mesurement.
 */
#define ADC_SAMPLES       25


/*
 * 100nF AREF buffer capacitor
 * - uncomment to enable if your board has one
 */
//#define HW_AREF_CAP


// --- Color Definitions for TFTeSPI ---
// Ensure TFT_eSPI.h is included in your main .ino file or here.
#include <TFT_eSPI.h>

#define COLOR_BACKGROUND  TFT_BLACK
#define COLOR_FOREGROUND  TFT_WHITE
#define COLOR_HIGHLIGHT   TFT_YELLOW // Used for highlighting elements

// Additional colors if UI_COLORED_TITLES, UI_COLORED_CURSOR, UI_COLORED_VALUES are enabled
#define COLOR_TITLE       TFT_CYAN
#define COLOR_CURSOR      TFT_BLUE
#define COLOR_VALUE       TFT_GREEN

// Probe specific colors if UI_PROBE_COLORS is enabled
#define COLOR_PROBE_1     TFT_RED
#define COLOR_PROBE_2     TFT_GREEN
#define COLOR_PROBE_3     TFT_BLUE