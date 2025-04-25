#include "adc.h"
#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
#include "lcd.h"

//! Global variables
uint16_t lastCaptured;

uint16_t* bufferStart;
uint8_t bufferSize;
uint8_t bufferIndex;

/*! \brief This method initializes the necessary registers for using the ADC module. \n
 * Reference voltage:    internal \n
 * Input channel:        PORTA0 \n
 * Conversion frequency: 156kHz
 */
void initAdc(void) {
    // Init DDRA0 as input
    // Set PA0 as input (clear PA0 bit in DDRA register)
    DDRA &= ~_BV(PA0);  // Clearing the PA0 bit in DDRA sets PA0 as an input

    // Set up the ADC: Use internal reference voltage and select ADC0 as the input
    // REFS1:0 = 01 for internal reference voltage, ADLAR = 0 for right-adjusted result
    ADMUX = _BV(REFS0); // Set REFS0 bit for internal reference, REFS1 remains 0

    // Configure ADC control settings
    // Enable ADC (ADEN = 1), set ADC prescaler to 128 (ADPS2:0 = 111), disable interrupts and auto-triggering
    ADCSRA = _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);

}

/*! \brief Executes one conversion of the ADC and returns its value.
 *
 * \return The converted voltage (0 = 0V, 1023 = AVCC)
 */
uint16_t getAdcValue() {
    // Start the conversion
    ADCSRA |= _BV(ADSC); // Set ADSC bit in ADCSRA register to start the ADC conversion

    // Wait until the conversion has finished (ADSC will be cleared when done)
    while (ADCSRA & _BV(ADSC)) {} // Check if ADSC bit is still set

    // Store the result of the ADC conversion
    lastCaptured = ADCL | ((uint16_t)ADCH << 8); // Combine the lower and upper 8 bits of the ADC result

    // Return the ADC result
    return lastCaptured;

}

/*! \brief Returns the size of the buffer which stores voltage values.
 *
 * \return The size of the buffer which stores voltage values.
 */
uint8_t getBufferSize() {
    return bufferSize;
}

/*! \brief Returns the current index of the buffer which stores voltage values.
 *
 * \return The current index of the buffer which stores voltage values.
 */
uint8_t getBufferIndex() {
    return bufferIndex;
}

/*! \brief Stores the last captured voltage.
 *
 */

// Function to allocate memory for the buffer if it's not already allocated
void allocateBufferIfNeeded(void) {
    // Check if the bufferStart pointer is NULL, meaning memory hasn't been allocated
    if (bufferStart == NULL) {
        // Allocate memory for 100 voltage readings (each of type uint16_t, 2 bytes per reading)
        bufferStart = malloc(100 * sizeof(uint16_t)); 
        
        // Set the buffer size to 100, indicating the number of readings that can be stored
        bufferSize = 100;  
    }
}



void storeVoltage(void) { //Messwert speichern
	//Wenn Puffergrosse = 0, muss erst mit malloc allozieren
    allocateBufferIfNeeded();
	
	//bufferIndex: nachste freie Index im Puffer 
	//Ist bufferIndex gleich oder grosser als puffergrosse, wird kein neuer Spannungswert gespeichert werden 
	//Andernfalls zuletzt gemessene Spannungnswert 'lastCaptured' an diese Position gespeichert 
		//Dann soll bufferIndex auch inkrementieren 
        if (bufferIndex < bufferSize) {
            // Get the latest ADC value
            lastCaptured = getAdcValue();
            
            // Store the ADC value at the current bufferIndex position
            *(bufferStart + bufferIndex) = lastCaptured;
            
            // Increment the bufferIndex to the next position for future readings
            bufferIndex++;
        }
}

/*! \brief Returns the voltage value with the passed index.
 *
 * \param ind   Index of the voltage value.
 * \return      The voltage value with index ind.
 */
uint16_t getStoredVoltage(uint8_t ind) {
    // If the requested index is within the valid range
    if (index < bufferIndex) {
        // Retrieve the voltage value stored at that index
        uint16_t voltage = *(bufferStart + index);
        return voltage;  // Return the stored voltage value
    }
    // Return 0 if the index is invalid (out of range)
    return 0;
}
