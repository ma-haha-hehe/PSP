#include "os_input.h"
#include "R2R.h"
#include "trackingWandler.h"

#include <util/delay.h>
#include <avr/io.h>
#include <stdbool.h>

// Global tracking variables
uint8_t voltageRef = 0x00; // Digital value for R-2R DAC
bool increaseRef = false;  // Direction flag for adjusting reference voltage

void trackingWandler(void) {
    // -------------------- Initialization --------------------
    R2R_init();           // Initialize R-2R DAC and LED ports
    os_initInput();       // Initialize input for button on Port C (C1)

    // -------------------- Main Loop --------------------
    while (1) {
        // Step 1: Output reference voltage to R-2R DAC
        PORTB = voltageRef;

        // Step 2: Read comparator output (Pin C0) to decide adjustment direction
        // C0 = 0 ¡ú Uref < Umess ¡ú need to increase ref
        // C0 = 1 ¡ú Uref >= Umess ¡ú need to decrease ref
        increaseRef = ((PINC & (1 << PC0)) == 0);

        // Step 3: Wait for button press on Pin C1 to start tracking
        if (os_getInput() == 0b10) {
            PORTA = 0xFF;  // LED off initially (active low)

            if (increaseRef) {
                // Uref too low ¡ú increment until comparator says it's enough
                while ((PINC & (1 << PC0)) == 0) {
                    if (voltageRef == 0xFF) break; // prevent overflow
                    voltageRef++;

                    PORTA = ~voltageRef;  // Update LED bar (active low)
                    PORTB = voltageRef;   // Update R-2R DAC output
                    _delay_ms(50);
                }

            } else {
                // Uref too high ¡ú decrement until comparator says it's low enough
                while ((PINC & (1 << PC0)) != 0) {
                    if (voltageRef == 0x00) break; // prevent underflow
                    voltageRef--;

                    PORTA = ~voltageRef;  // Update LED bar (active low)
                    PORTB = voltageRef;   // Update R-2R DAC output
                    _delay_ms(50);
                }
            }
        }
    }
}

/*
  Summary of Connections and Logic:

  - PORTB ¡ú Sends 8-bit digital value to R-2R DAC
  - R-2R DAC ¡ú Outputs analog voltage Uref
  - Uref ¡ú Feeds into Comparator input on Pin C0
  - Umess ¡ú Analog input voltage (e.g., from sensor)
  - Comparator Output (C0):
      - 0 if Uref < Umess
      - 1 if Uref >= Umess
  - Button on C1 ¡ú Triggers measurement process
  - PORTA ¡ú LED output to show ref value (inverted: 0 = LED ON)
*/
