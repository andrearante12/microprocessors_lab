#include <avr/io.h>
#include <stdint.h>

/* ---------- prototypes ---------- */
static unsigned char read_keypad(void);   

int main(void) {
	
	DDRA = 0x00; // set PORTA for input
	DDRB = 0xFF; // set PORTB for output
	
	unsigned char k;

    while (1) {
        char k = read_keypad();   
        display_7led(k);  
    }
}

unsigned char read_keypad(void) {
	unsigned char x;

	DDRA  = 0b11110000;   // columns = outputs, rows = inputs
	PORTA = 0b11111111;   

	PORTA = 0b10111111;   // column 1 
	asm volatile("nop");
	x = PINA;
	if ((x & 0b00001000) == 0) return '1';  
	if ((x & 0b00000100) == 0) return '4';  
	if ((x & 0b00000010) == 0) return '7';  

	PORTA = 0b11011111;   // column 2
	asm volatile("nop");
	x = PINA;
	if ((x & 0b00001000) == 0) return '2';  
	if ((x & 0b00000100) == 0) return '5'; 
	if ((x & 0b00000010) == 0) return '8';  
	if ((x & 0b00000001) == 0) return '0';  

	PORTA = 0b11101111;   // column 3
	asm volatile("nop");
	x = PINA;
	if ((x & 0b00001000) == 0) return '3';  
	if ((x & 0b00000100) == 0) return '6';  
	if ((x & 0b00000010) == 0) return '9';  

	return 0; 
}

void display_7led(unsigned char k) {
	if (k == '0')      PORTB = 0b11000000; // 0
	else if (k == '1') PORTB = 0b11111001; // 1
	else if (k == '2') PORTB = 0b10100100; // 2
	else if (k == '3') PORTB = 0b10110000; // 3
	else if (k == '4') PORTB = 0b10011001; // 4
	else if (k == '5') PORTB = 0b10010010; // 5
	else if (k == '6') PORTB = 0b10000010; // 6
	else if (k == '7') PORTB = 0b11111000; // 7
	else if (k == '8') PORTB = 0b10000000; // 8
	else if (k == '9') PORTB = 0b10010000; // 9
	else               PORTB = 0b11111111;
}
