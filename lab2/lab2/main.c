#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>


void serial_init(void){
	// Asynchronous mode, no parity, 1 stop bit, 8 data bits
	UCSRC = 0b10000110;
	// Normal speed, disable multi-proc
	UCSRA = 0b00000000;
	// Baud rate 1200bps, assuming 1MHz clock
	UBRRL = 0x33;
	UBRRH = 0x00;
	// Enable Tx and Rx, disable interrupts
	UCSRB = 0b00011000;
}
/*
void serial_init_1(void){
	// Asynchronous mode, no parity, 1 stop bit, 8 data bits
	UCSRC = 0b10000110;
	// Normal speed, disable multi-proc
	UCSRA = 0b00000000;
	// Baud rate 2400bps, assuming 1MHz clock
	UBRRL = 0x19;
	UBRRH = 0x00;
	// Enable Tx and Rx, disable interrupts
	UCSRB = 0b00011000;
}

void serial_init_2(void){
	// Asynchronous mode, even parity, 2 stop bit, 1 start bit, 8 data bits
	UCSRC = 0b10101110;
	// Normal speed, disable multi-proc
	UCSRA = 0b00000000;
	// Baud rate 4800bps, assuming 4MHz clock
	UBRRL = 0x33;
	UBRRH = 0x00;
	// Enable Tx and Rx, disable interrupts
	UCSRB = 0b00011000;
}
*/

void serial_send(unsigned char data){
	// Wait until UDRE flag = 1
	while ((UCSRA & (1 << UDRE)) == 0x00){;}
	// Write char to UDR for transmission
	UDR = data;
}

unsigned char serial_receive(void){
	// Wait until RXC flag = 1
	while ((UCSRA & (1 << RXC)) == 0x00){;}
	// Read the received char from UDR
	return (UDR);
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
	//else               PORTB = 0b11111111;
}

int main(void) {
	
	DDRA = 0x00; // set PORTA for input
	DDRB = 0xFF; // set PORTB for output
	
	unsigned char k;
	
	serial_init();
	
	

	while (1) {
		
		// Task 1
		//char k = read_keypad();
		//serial_send(k);
		
		// Task 2
		//char k = serial_receive();
		//display_7led(k);
		
		// Task 3
		
		unsigned char a, b;
		int a_int, b_int;
		char c;
		
		stdout = fdevopen(serial_send, NULL);
		stdin = fdevopen(NULL, serial_receive);
		
		while (1) {
			
			printf("Formatted input/output via serial port. \n\r Please enter two integer values: \n\r");
			printf("Integer a = ");

			if (scanf("%c", &a) == 1) {   // check input success
				a_int = (int) a - 48;
			}
			
			scanf("%c", &c);
			
			printf("\n\Integer b = ");
			if (scanf("%c", &b) == 1) {   // check input success
				b_int = (int) b - 48;
			}
			
			printf("a + b = %d \n\r", a_int + b_int);
			printf("a - b = %d \n\r", a_int - b_int);
			printf("a * b = %d \n\r", a_int * b_int);
			
			break;
							
		}
		
		
		
	}
}