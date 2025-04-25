
#include "R2R.h"
#include <avr/io.h>
#include <stdint.h>

void manuell(void){
	//Zuerst Port Initialisieren
	R2R_init();
	
	while(1){
		//Wenn DIP ON, dann ist Signal 0
		//Wenn DIP OFF, dann ist Signal 1
		//Wenn Widerstand 0, dann leuchtet LED
		//DIP follows LED design (active low) no need for inversion 
		PORTA = PIND; 
		//Wenn Widerstand 1, dann fliesst Strom, we need inversion for R2R cuz its not active low design 
		PORTB = ~PIND;
	}
}

void R2R_init(void){
	//Zuerst Port Initialisieren
	/*
	Port A A0 LED f¨¹r Wandlungsergebnis (Bit 1)
		A1 LED f¨¹r Wandlungsergebnis (Bit 2)
		A2 LED f¨¹r Wandlungsergebnis (Bit 3)
		A3 LED f¨¹r Wandlungsergebnis (Bit 4)
		A4 LED f¨¹r Wandlungsergebnis (Bit 5)
		A5 LED f¨¹r Wandlungsergebnis (Bit 6)
		A6 LED f¨¹r Wandlungsergebnis (Bit 7)
		A7 LED f¨¹r Wandlungsergebnis (Bit 8)
		Port B B0 R-2R-Netzwerk (Ausgabe Steuersignale)
		B1 R-2R-Netzwerk (Ausgabe Steuersignale)
		B2 R-2R-Netzwerk (Ausgabe Steuersignale)
		B3 R-2R-Netzwerk (Ausgabe Steuersignale)
		B4 R-2R-Netzwerk (Ausgabe Steuersignale)
		B5 R-2R-Netzwerk (Ausgabe Steuersignale)
		B6 R-2R-Netzwerk (Ausgabe Steuersignale)
		B7 R-2R-Netzwerk (Ausgabe Steuersignale)
		Port D D0 DIP-Schalter 1 (Eingabe f¨¹r manuelle Wandlung)
		D1 DIP-Schalter 2 (Eingabe f¨¹r manuelle Wandlung)
		D2 DIP-Schalter 3 (Eingabe f¨¹r manuelle Wandlung)
		D3 DIP-Schalter 4 (Eingabe f¨¹r manuelle Wandlung)
		D4 DIP-Schalter 5 (Eingabe f¨¹r manuelle Wandlung)
		D5 DIP-Schalter 6 (Eingabe f¨¹r manuelle Wandlung)
		D6 DIP-Schalter 7 (Eingabe f¨¹r manuelle Wandlung)
		D7 DIP-Schalter 8 (Eingabe f¨¹r manuelle Wandlung)
	*/
	
	DDRA=0xFF; //(Ausgang)
	PORTA= 0xFF; //0 fur leuchten
	DDRB=0xFF; //(Ausgang)
	DDRD= 0x00; //(Eingang)
	PORTD= 0xFF; //EIngang, also muss Widerstand aktivieren
}