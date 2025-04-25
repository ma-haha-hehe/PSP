#include "os_input.h"

#include <avr/io.h>
#include <stdint.h>

/*! \file
 *  Functions for reading button input from the evaluation board.
 */

/*!
 *  Reads the current state of the buttons on the evaluation board.
 *
 *  \returns A byte where each relevant bit indicates if a button is pressed (1) or not (0).
 *  Example:
 *      - One button pressed:      00000001
 *      - No button pressed:       00000000
 *      - Buttons 1, 3, and 4:     00001101
 */
uint8_t os_getInput(void) {
    // Read the PINC register (actual logic level on PORTC input pins)
    // Invert it so pressed buttons (which pull the pin low) become 1.
    uint8_t pinInverted = ~PINC;

    // Extract PC6 and PC7 (Up and ESC buttons).
    // Bit 6 (PC6) becomes bit 2, Bit 7 (PC7) becomes bit 3 after shifting.
    uint8_t high = (pinInverted & ((1 << PC6) | (1 << PC7))) >> 4;

    // Extract PC0 and PC1 (Enter and Down buttons), keep them in bits 0 and 1.
    uint8_t low = pinInverted & ((1 << PC0) | (1 << PC1));

    // Merge both into one byte: bits 0-3 contain the 4 button states.
    return high | low;
}

/*!
 *  Configures PORTC for button input with pull-up resistors.
 */
void os_initInput() {
    // Set PC0, PC1, PC6, PC7 as inputs by clearing their bits in DDRC
    DDRC &= ~((1 << PC0) | (1 << PC1) | (1 << PC6) | (1 << PC7));

    // Enable internal pull-up resistors by setting bits in PORTC
    // This ensures the pins read HIGH when buttons are not pressed
    PORTC |= (1 << PC0) | (1 << PC1) | (1 << PC6) | (1 << PC7);
}

/*!
 *  Blocks execution until all buttons are released.
 */
void os_waitForNoInput() {
    while (os_getInput() != 0) {
        // Wait until no button is pressed
    }
}

/*!
 *  Blocks execution until at least one button is pressed.
 */
void os_waitForInput() {
    while (os_getInput() == 0) {
        // Wait until a button is pressed
    }
}
