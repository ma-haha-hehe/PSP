#include "os_input.h"
#include <avr/io.h>
#include <stdint.h>

/*! \file
 *  Handles user button inputs from Port C on the evaluation board.
 *  Provides utility functions to read input, wait for press/release, and initialize configuration.
 */

/*!
 *  Reads the state of user buttons connected to Port C.
 *  
 *  The hardware setup:
 *    - Buttons are connected to:
 *        C1 (Down), C6 (Up), and C7 (ESC).
 *    - Buttons use pull-up logic:
 *        - When released: PINC reads 1
 *        - When pressed:  PINC reads 0
 *
 *  This function returns a bitmask combining those button states.
 *
 *  \return Bitmask where each bit represents one button:
 *          Bit 0 = C1 (Down)
 *          Bit 1 = C6 (Up)
 *          Bit 2 = C7 (ESC)
 */
uint8_t os_getInput(void) {
    // Read PINC, invert bits because buttons are active-low (0 when pressed)
    // Mask bits C7, C6, and C1 using 0b11000010:
    // - C7 (bit 7) ¡ú Up
    // - C6 (bit 6) ¡ú ESC
    // - C1 (bit 1) ¡ú Down
    uint8_t invertedPins = ~PINC;

    // Extract C7 (bit 7) and C6 (bit 6), shift to right to put them in bits 3 and 2
    // Example: bit 7 becomes bit 3, bit 6 becomes bit 2
    uint8_t upperButtons = (invertedPins & 0b11000000) >> 4;

    // Extract C1 (bit 1), this remains in position
    uint8_t downButton = invertedPins & 0b00000010;

    // Combine shifted upper button bits and C1 into a single 3-bit result
    uint8_t pinState = upperButtons | downButton;

    return pinState;
}

/*!
 *  Initializes Port C for reading button inputs.
 *  
 *  Configuration:
 *    - C0, C1, C6, C7 used for input (buttons attached).
 *    - Pins are set as input by writing 0 to DDRC.
 *    - Pull-up resistors are enabled by writing 1 to PORTC.
 * Bits 7, 6, 1, and 0 are set to 1 in PORTC

    Since those same pins are inputs (DDRC = 0 for those bits), the MCU enables internal pull-ups on those pins.

    Why we need pull-ups for buttons
    Without pull-ups, the input pin "floats" when the button is not pressed ¡ª it reads random voltage = unstable behavior.

    With pull-ups:

    When button not pressed: input = pulled HIGH by resistor = 1

    When button pressed: input = pulled LOW (connected to GND) = 0

    So we get:
 */
void os_initInput() {
    // Set DDRC bits 7, 6, 1, 0 to 0 ¡ú input
    DDRC &= ~(0b11000011);

    // Set PORTC bits 7, 6, 1, 0 to 1 ¡ú enable internal pull-ups
    //On AVR microcontrollers (like ATmega), setting a pin in PORTx to 1 while it's configured as an input (DDRx = 0) activates its internal pull-up resistor.
    PORTC |= 0b11000011;
}


/*!
 *  Waits in a loop until all buttons are released.
 *  
 *  Assumes that 0b00000001 represents the idle state (no button except C0 is pressed).
 *  C0 is ignored in detection and always considered 'pressed'.
 */
void os_waitForNoInput() {
    // Idle means only C0 active (bit 0 set)
    while (os_getInput() != 0b00000001) {
        // Keep waiting until all user-relevant buttons are unpressed
    }
}

/*!
 *  Waits in a loop until any button is pressed.
 *  
 *  Will block as long as either:
 *    - No button is pressed (0b00000000)
 *    - Only C0 is active (0b00000001)
 */
void os_waitForInput() {
    // Wait while nothing or just C0 is pressed
    while (os_getInput() == 0b00000000 || os_getInput() == 0b00000001) {
        // Keep waiting until a real button is pressed
    }
}
