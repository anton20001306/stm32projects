/*
 * keypad.h
 *
 *  Bare-metal 4x4 matrix keypad driver (STM32F446).
 *
 *  Wiring (all on GPIOC):
 *    Columns : PC0, PC1, PC2, PC3 -> inputs with pull-up
 *    Rows    : PC4, PC5, PC6, PC7 -> outputs, idle HIGH
 *
 *  Scanning: one row is driven LOW at a time, the columns are read.
 *  A pressed key pulls its column LOW while that row is LOW.
 */

#ifndef INC_KEYPAD_H_
#define INC_KEYPAD_H_

#include <stdint.h>

/* Column input pins (pull-up) */
#define KP_COL0_PIN   0U
#define KP_COL1_PIN   1U
#define KP_COL2_PIN   2U
#define KP_COL3_PIN   3U

/* Row output pins */
#define KP_ROW0_PIN   4U
#define KP_ROW1_PIN   5U
#define KP_ROW2_PIN   6U
#define KP_ROW3_PIN   7U

/*
 * Initialise the keypad GPIOs at register level:
 *   - enables GPIOC clock
 *   - PC0..PC3 : input, pull-up
 *   - PC4..PC7 : output push-pull, idle HIGH
 */
void keypad_init(void);

/*
 * Scan the matrix once by driving each row LOW in turn and reading the columns.
 * Returns the character of the first pressed key, or 0 ('\0') if none.
 */
char keypad_read(void);

/*
 * Map a pressed key to a MAVLink/ArduPilot console command and send it over
 * UART (via Send_String). Unmapped keys are ignored.
 */
void keypad_send_command(char key);

#endif /* INC_KEYPAD_H_ */
