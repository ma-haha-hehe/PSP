#include "R2R.h"
#include "SAR.h"
#include "trackingWandler.h"

#include <avr/io.h>


int main(void) {
	/* Replace with your application code */
	int option = 3;
	while (1) {
		switch(option){
			case 1: // R2R
			manuell();
			
			case 2: //SAR
			SAR();
			
			case 3: //Tracking Wandler
			trackingWandler();
			
		}
	}
}
