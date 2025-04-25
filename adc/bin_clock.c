#include "bin_clock.h"
#include <avr/io.h>
#include <avr/interrupt.h>

//! Global variables
uint16_t milisec;
uint8_t sec;
uint8_t minute;
uint8_t hour; 

/*!
 * \return The milliseconds counter of the current time.
 */
uint16_t getMilliseconds() {
    return milisec;
}

/*!
 * \return The seconds counter of the current time.
 */
uint8_t getSeconds() {
    return sec;
}

/*!
 * \return The minutes counter of the current time.
 */
uint8_t getMinutes() {
    return minute;
}

/*!
 * \return The hour counter of the current time.
 */
uint8_t getHours() {
    return hour;
}

/*!
 *  Initializes the binary clock (ISR and global variables)
 */
void initClock(void) {
    // Set timer mode to CTC
    TCCR0A &= ~(1 << WGM00);
    TCCR0A |= (1 << WGM01);
    TCCR0B &= ~(1 << WGM02);

    // Set prescaler to 1024
    TCCR0B |= (1 << CS02) | (1 << CS00);
    TCCR0B &= ~(1 << CS01);

    // Set compare register to 195 -> match every 10ms
    OCR0A = 195;

    // Init variables
	// (1024/{20*10^6}) * 195 = 0,01sec
    // Initialize time-related variables with default values
    milisec = 0b00000000;  // Milliseconds set to 0
    sec = 0b00000000;      // Seconds set to 0
    minute = 0b00000000;   // Minutes set to 0 (test, can change later)
    hour = 0b00001100;     // Start time set to 12:00 (in binary for 12)

    // Avoid starting with 0:00 - 0:59 for the initial time setup


    // Enable timer and global interrupts
    TIMSK0 |= (1 << OCIE0A);
    sei(); //Global Interrupt 
}

/*!
 *  Updates the global variables to get a valid 12h-time
 */
void updateClock(void) {

        // Increase milliseconds by 10 each time this function is called
        milisec += 10;  // Increment milliseconds by 10 (in decimal)
    
       // Check if milliseconds hit 1000 (1 second)
    if (milisec >= 1000) {
        milisec = 0;  // Reset milliseconds
        sec++;        // Increment seconds
    }

    // Check if seconds hit 60
    if (sec >= 60) {
        sec = 0;      // Reset seconds
        minute++;     // Increment minutes
    }

    // Check if minutes hit 60
    if (minute >= 60) {
        minute = 0;   // Reset minutes
        hour++;       // Increment hours
    }

    // Check if hours hit 13 (to avoid 0:00 - 0:59)
    if (hour >= 13) {
        hour = 1;     // Reset hours to 1
    }

    
}

/*!
 *  ISR to increase second-counter of the clock
 */
ISR(TIMER0_COMPA_vect) {
    updateClock();
}
