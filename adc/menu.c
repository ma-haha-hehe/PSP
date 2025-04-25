#include "menu.h"
#include "os_input.h"
#include "bin_clock.h"
#include "lcd.h"
#include "led.h"
#include "adc.h"
#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>


/*!
 *  Hello world program.
 *  Shows the string 'Hello World!' on the display.
 */

//! Global variables
uint16_t miliseconds;
uint8_t sec;
uint8_t minute;
uint8_t hour;
uint16_t combinedTime;

uint8_t isEscPressed(void) {
    return (os_getInput() == 0b00001000);
}

void lcd_writePaddedDec(uint8_t value, uint8_t minDigits) {
	if (value < 10 && minDigits >= 2) {
		lcd_writeDec(0);
	}
	lcd_writeDec(value);
}

void lcd_writePaddedMilisec(uint8_t value) {
	if (value < 100) {
		lcd_writeDec(0); // e.g. 050
	}
	lcd_writeDec(value);
}

void updateTime() {
	sec = getSeconds();
	minute = getMinutes();
	hour = getHours();
	miliseconds = getMilliseconds();
}

void writePaddedDec(uint16_t value, uint8_t width) {
	if (value < 10 && width >= 3) {
		lcd_writeProgString(PSTR("00"));
	} else if (value < 100 && width >= 3) {
		lcd_writeProgString(PSTR("0"));
	}
	lcd_writeDec(value);
}

// Computes the LED display value based on ADC result
// - Each LED lights up for every 68 ADC units
// - The first LED lights up at ADC value >= 68
// - Each subsequent LED lights up for every 68 units of ADC value
// - Maximum ADC value (1023) lights up all 16 LEDs
// - The result is a 16-bit value where each bit represents an LED (0 = off, 1 = on)

uint16_t computeLedValue(uint16_t adcResult) {
	uint16_t ledValue = 0;
	while (adcResult >= 68) {
		ledValue = (ledValue << 1) | 0x0002;
		adcResult -= 68;
	}
	return ~ledValue;
}

void updateSpeicherCounter(uint8_t *counter, uint8_t input) {
	if (input == 0b00000100) { // UP
		*counter = (*counter != 100) ? (*counter + 1) : 0;
	} else if (input == 0b00000010) { // DOWN
		*counter = (*counter != 0) ? (*counter - 1) : 100;
	}
}

void checkAndStoreVoltage(uint8_t input) {
    if (input == 0b00000001) { // If ENTER button (0b00000001) is pressed
        storeVoltage(); // Store the current voltage
    }
}


void helloWorld(void) {
	// Repeat until ESC gets pressed
	lcd_init();
	lcd_clear();

	while(!isEscPressed()) { //solange ESC nicht gedruckt, wiederholen
		lcd_writeProgString(PSTR("HELLO WORLD!"));
		_delay_ms(500);
		lcd_clear();
		if (isEscPressed()) {
			break;
		}
		_delay_ms(500);
	}
	
}

/*!
 *  Shows the clock on the display and a binary clock on the led bar.
 */
void displayClock(void) {
	while(!isEscPressed()) { //solange ESC nicht gedruckt, wiederholen
	
		updateTime();
		
		// Update the combined time for the binary clock on the LED bar

		// Shift and combine the hour value into the higher bits of combinedTime
		combinedTime = (combinedTime | hour) << 6;  // Add hour to the leftmost bits (hour << 6)

		// Shift and combine the minute value into the next bits of combinedTime
		combinedTime = (combinedTime | minute) << 6;  // Add minute after shifting the previous hour data

		// Combine the second value into the least significant bits of combinedTime
		combinedTime = (combinedTime | sec);  // Add seconds to the least significant bits

		// Display the final combined time (in binary) on the LED bar (invert bits for correct display)
		setLedBar(~combinedTime);  // Invert combinedTime to match the LED bar logic

		    
		lcd_clear();
		lcd_writePaddedDec(hour, 2);
		lcd_writeChar(':');
		lcd_writePaddedDec(minute, 2);
		lcd_writeChar(':');
		lcd_writePaddedDec(sec, 2);
		lcd_writeChar(':');
		lcd_writePaddedMilisec(milisec);
			
		    
		    
		// Pro 10milisec wird Zeit erneuert
		// Also am besten vllt sollen wir 10 milisec warten
		    
		_delay_ms(10);
		    
	
	}

}



/*!
 *  Shows the stored voltage values in the second line of the display.
 */
void displayVoltageBuffer(uint8_t displayIndex) {
	lcd_line2();
	
	// Write padded displayIndex like "007", "045", etc.
	writePaddedDec(displayIndex, 3);

	lcd_writeProgString(PSTR("/100: "));
	uint16_t adcResult = getStoredVoltage(displayIndex);
	lcd_writeVoltage(adcResult, 1023, 5);

}

/*!
 *  Shows the ADC value on the display and on the led bar.
 */

void displayVoltageLabel(void) {
	lcd_clear();                     // Reset the display for the latest content
	lcd_line1();                     // Position the cursor on the first line
	lcd_writeProgString(PSTR("Voltage: ")); // Display the label "Voltage: "
}

void displayAdc(void) {
	uint8_t SpeicherCounter =0;
	while (!isEscPressed()) { // Loop until ESC is pressed

		displayVoltageLabel(); // Display "Voltage: " on the screen

		uint16_t adcResult = getAdcValue(); // Read analog value from sensor

		lcd_writeVoltage(adcResult, 1023, 5); // Display voltage (scaled to 5V, 10-bit resolution)

		// Compute and display binary LED representation
		uint16_t invertedLedValue = computeLedValue(adcResult);
		setLedBar(invertedLedValue); // Display inverted for correct visual output

		// If buffer is allocated, show stored voltage at current index
		if (getBufferSize() != 0) {
			displayVoltageBuffer(SpeicherCounter);
		}

		uint8_t input = os_getInput(); // Get button input

		// Check if ENTER button is pressed, then store voltage
		checkAndStoreVoltage(input); 

		// Handle UP/DOWN changes for SpeicherCounter
		updateSpeicherCounter(&SpeicherCounter, input); 

		_delay_ms(100); // Wait 100ms before next loop (refresh rate)
	}
	
}

/*! \brief Starts the passed program
 *
 * \param programIndex Index of the program to start.
 */
void start(uint8_t programIndex) {
    // Initialize and start the passed 'program'
    switch (programIndex) {
        case 0:
            lcd_clear();
            helloWorld();
            break;
        case 1:
            activateLedMask = 0xFFFF; // Use all LEDs
            initLedBar();
            initClock();
            displayClock();
            break;
        case 2:
            activateLedMask = 0xFFFE; // Don't use LED 0
            initLedBar();
            initAdc();
            displayAdc();
            break;
        default:
            break;
    }

    // Do not resume to the menu until all buttons are released
    os_waitForNoInput();
}

/*!
 *  Shows a user menu on the display which allows to start subprograms.
 */
void showMenu(void) {
    uint8_t pageIndex = 0;

    while (1) {
        lcd_clear();
        lcd_writeProgString(PSTR("Select:"));
        lcd_line2();

        switch (pageIndex) {
            case 0:
                lcd_writeProgString(PSTR("1: Hello world"));
                break;
            case 1:
                lcd_writeProgString(PSTR("2: Binary clock"));
                break;
            case 2:
                lcd_writeProgString(PSTR("3: Internal ADC"));
                break;
            default:
                lcd_writeProgString(PSTR("----------------"));
                break;
        }

        os_waitForInput();
        if (os_getInput() == 0b00000001) { // Enter
            os_waitForNoInput();
            start(pageIndex);
        } else if (os_getInput() == 0b00000100) { // Up
            os_waitForNoInput();
            pageIndex = (pageIndex + 1) % 3;
        } else if (os_getInput() == 0b00000010) { // Down
            os_waitForNoInput();
            if (pageIndex == 0) {
                pageIndex = 2;
            } else {
                pageIndex--;
            }
        }
    }
}
