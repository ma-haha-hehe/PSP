#include "led.h"
#include <avr/io.h>

uint16_t activateLedMask = 0xFFFF;

/*!
 *  Initializes the led bar. Note: All PORTs will be set to output.
 */
void initLedBar(void) {
    // Configure Port A and Port D for LED output
    DDRA = 255;   // Set all Port A pins as output
    PORTA = 255;  // Turn all Port A LEDs off initially (1 = off)

    DDRD = 255;   // Set all Port D pins as output
    PORTD = 255;  // Turn all Port D LEDs off initially (1 = off)
}

/*!
 *  Sets the passed value as states of the LED bar (0 = on, 1 = off).
 */
void setLedBar(uint16_t value) {
    PORTD = 255;  // Default all LEDs on Port D to off
    PORTA = 255;  // Default all LEDs on Port A to off

    // Map the 16-bit 'value' to corresponding ports:
    // High byte (hhhhmmmm) → PORTD
    // Low byte (mmssssss)  → PORTA
    PORTD = (value >> 8); // Send high 8 bits to PORTD
    PORTA = value;        // Send low 8 bits to PORTA
}


