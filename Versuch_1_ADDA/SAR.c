#include "os_input.h"  // Include the header file for input handling
#include "R2R.h"        // Include the header file for R-2R DAC handling
#include "SAR.h"        // Include the header file for SAW-related functions
#include <util/delay.h> // Include the delay function for timing control
#include <avr/io.h>     // Include the AVR I/O library for working with ports and pins
#include <stdbool.h>    // Include the standard boolean type (true/false)

// Global variables
uint8_t dacValue = 0;  // Variable to store the current value for the DAC (digital value sent to R-2R)
bool adjustFlag = false; // A flag to decide whether the DAC value should be increased or decreased

// Function to initialize necessary components (like DAC and input ports)
void setup(void) {
	R2R_init();  // Initialize the R-2R DAC (digital-to-analog converter)
	os_initInput();  // Initialize the input system (for reading button presses)
}

// Function to check the comparator input (Pin C0) to decide whether we need to increase or decrease the DAC value
void checkComparator(void) {
	// Read the comparator output from Pin C0
	// If C0 is low (0), then Uref (reference voltage) is smaller than Umess (measured voltage), so we need to increase Uref
	if ((PINC & (1 << PC0)) == 0) {
		adjustFlag = false;  // Set the flag to false, indicating we need to increase the DAC value
		} else {
		adjustFlag = true;   // Set the flag to true, indicating we need to decrease the DAC value
	}
}

// Function to update the DAC output and LED bar display
void updateDAC(void) {
	PORTB = dacValue;  // Send the current DAC value to PORTB, which is connected to the R-2R DAC

	// Update the LED bar (PORTA) to show the current DAC value
	// Inverted: if a bit in DAC value is 0, the corresponding LED in PORTA will be ON
	PORTA = ~dacValue;  // Invert the DAC value to control the LED state (active low)
}

// Function to adjust the DAC value by incrementing or decrementing it
void adjustDAC(void) {
	// Start with the 8 most significant bits (from 7 down to 0)
	for (int i = 7; i >= 0; i--) {
		uint8_t bitMask = (1 << i);  // Create a bitmask for the current bit position (e.g., 10000000 for bit 7)

		// Check if we need to increase or decrease the DAC value based on the flag
		if (!adjustFlag) {
			dacValue |= bitMask; // Set the bit (increase the DAC value)
			} else {
			dacValue &= ~bitMask; // Clear the bit (decrease the DAC value)
		}

		// Update the DAC and LED bar after modifying the DAC value
		updateDAC();

		// Wait for the system to stabilize for 50 milliseconds before checking the comparator again
		_delay_ms(50);

		// Check the comparator to see if we need to continue adjusting the DAC value
		checkComparator();
	}
}

// Main function to start the process
void SAR(void) {
	// Initialize all necessary components
	setup();

	// Main loop that continuously checks for user input
	while (1) {
		// Wait for button press on Pin C1 (when button is pressed, os_getInput returns 0b10)
		if (os_getInput() == 0b10) {
			// Reset the DAC value to 0 at the start
			dacValue = 0x00;

			// Start adjusting the DAC value based on comparator readings
			adjustDAC();
		}
	}
}

