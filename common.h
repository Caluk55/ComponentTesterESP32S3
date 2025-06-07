/* ************************************************************************
 *
 * common header file
 *
 * (c) 2012-2024 by Markus Reschke
 * based on code from Markus Frejek and Karl-Heinz K�bbeler
 *
 * ************************************************************************ */


/*
 * include header files
 */

/* basic includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

/* AVR - Rimosse le inclusioni specifiche AVR per ESP32 */
// #include <avr/io.h>
// #include <util/delay.h>
// #include <avr/sleep.h>
// #include <avr/eeprom.h>
// #include <avr/pgmspace.h> // Questo include � ancora presente per pgm_read_byte_near, ma l'implementazione per ESP32 � gestita diversamente.
// #include <avr/wdt.h>
// #include <avr/interrupt.h>


/* source management */
#define COMMON_H



/* ************************************************************************
 * constants for operation and UI
 * ************************************************************************ */


/* UI feedback mode for TestKey() (bitfield) */
#define CURSOR_NONE           0b00000000     /* no cursor */
#define CURSOR_STEADY         0b00000001     /* steady cursor */
#define CURSOR_BLINK          0b00000010     /* blinking cursor */
#define CHECK_OP_MODE         0b00000100     /* consider operation mode */
#define CHECK_KEY_TWICE       0b00001000     /* check for two short presses of the test key */
#define CHECK_BAT             0b00010000     /* check battery */
#define CURSOR_TEXT           0b00100000     /* show hint instead of cursor */


/* keys (test push button etc.) */
#define KEY_NONE              0   /* no key or error */
#define KEY_TIMEOUT           0   /* timeout */
#define KEY_SHORT             1   /* test push button: short key press */
#define KEY_LONG              2   /* test push button: long key press */
#define KEY_TWICE             3   /* test push button: two short key presses */
#define KEY_RIGHT             4   /* rotary encoder: right turn */
                                        /* push buttons: increase */
#define KEY_LEFT              5   /* rotary encoder: left turn */
                                        /* push buttons: decrease */
#define KEY_INCDEC            6   /* push buttons: increase and decrease */

/* virtual keys */
#define KEY_COMMAND           100 /* remote command (from serial interface) */
#define KEY_MAINMENU          101 /* main menu */
#define KEY_POWER_ON          102 /* just powered up */
#define KEY_POWER_OFF         103 /* power off */
#define KEY_PROBE             104 /* probe component */
#define KEY_EXIT              105 /* exit (menu) */


/* operation mode/state flags (bitfield) */
#define OP_NONE               0b00000000     /* no flags */
#define OP_AUTOHOLD           0b00000001     /* auto-hold mode (instead of continuous) */
#define OP_EXT_REF            0b00000100     /* external voltage reference used */
#define OP_SPI                0b00001000     /* SPI is set up */
#define OP_I2C                0b00010000     /* I2C is set up */
#define OP_AUTOHOLD_TEMP      0b00100000     /* temporary auto-hold mode */


/* operation control/signaling flags (bitfield) */
#define OP_BREAK_KEY          0b00000001     /* exit key processing */
#define OP_OUT_LCD            0b00000010     /* output to LCD display */
#define OP_OUT_SER            0b00000100     /* output to TTL serial */
#define OP_RX_LOCKED          0b00001000     /* RX buffer locked */
#define OP_RX_OVERFLOW        0b00010000     /* RX buffer overflow */
#define OP_PWR_TIMEOUT        0b00100000     /* auto-power-off for auto-hold mode */


/* UI line modes (bitfield) */
#define LINE_STD              0b00000000     /* standard mode */
#define LINE_KEY              0b00000001     /* wait for key press */
#define LINE_KEEP             0b00000010     /* keep first line */


/* storage modes (ID and bitfield) */
#define STORAGE_LOAD          0b00000001     /* load adjustment values (ID) */
#define STORAGE_SAVE          0b00000010     /* save adjustment values (ID) */
#define STORAGE_SHORT         0b00000100     /* short menu (flag) */ 


/* SPI */
/* clock rate flags (bitfield) */
#define SPI_CLOCK_R0          0b00000001     /* divider bit 0 (SPR0) */
#define SPI_CLOCK_R1          0b00000010     /* divider bit 1 (SPR1) */
#define SPI_CLOCK_2X          0b00000100     /* double clock rate (SPI2X) */


/* I2C */
#define I2C_ERROR             0          /* bus error */
#define I2C_OK                1          /* operation done */
#define I2C_START             1          /* start condition */
#define I2C_REPEATED_START    2          /* repeated start condition */
#define I2C_DATA              1          /* data byte */
#define I2C_ADDRESS           2          /* address byte */
#define I2C_ACK               1          /* acknowledge */
#define I2C_NACK              2          /* not-acknowledge */


/* TTL serial */
/* control */
#define SER_RX_PAUSE          1          /* pause RX */
#define SER_RX_RESUME         2          /* resume RX */

/* special characters */
#define CHAR_XON              17         /* software flow control: XON */
#define CHAR_XOFF             19         /* software flow control: XOFF */


/* modes for probe pinout */
#define PROBES_PWM            0          /* PWM output */
#define PROBES_ESR            1          /* ESR measurement */
#define PROBES_RCL            2          /* monitoring RCL */
#define PROBES_RINGTESTER     3          /* ring tester */
#define PROBES_DIODE          4          /* diode */


/* E series */
#define E6                    6          /* E6 */
#define E12                   12         /* E12 */
#define E24                   24         /* E24 */
#define E48                   48         /* E48 */
#define E96                   96         /* E96 */
#define E192                  192        /* E192 */


/* alignment */
#define ALIGN_LEFT            3          /* align left */
#define ALIGN_RIGHT           4          /* align right */



/* ************************************************************************
 * constants for arrays in variables.h
 * ************************************************************************ */


/* string buffer sizes */
#define OUT_BUFFER_SIZE       12         /* 11 chars + terminating 0 */
#define RX_BUFFER_SIZE        11         /* 10 chars + terminating 0 */

/* number of entries in data tables */
#define NUM_PREFIXES          8          /* unit prefixes */
#define NUM_LARGE_CAP         46         /* large cap factors */
#define NUM_SMALL_CAP         9          /* small cap factors */
#define NUM_PWM_FREQ          8          /* PWM frequencies */
#define NUM_INDUCTOR          32         /* inductance factors */
#define NUM_TIMER1            5          /* Timer1 prescalers and bits */
#define NUM_PROBE_COLORS      3          /* probe colors */
#define NUM_E6                6          /* E6 norm values */
#define NUM_E12               12         /* E12 norm values */
#define NUM_E24               24         /* E24 norm values */
#define NUM_E96               96         /* E24 norm values */
#define NUM_COLOR_CODES       10         /* color codes */
#define NUM_EIA96_MULT        9          /* EIA-96 multiplier codes */
#define NUM_LOGIC_TYPES       6          /* logic families and */