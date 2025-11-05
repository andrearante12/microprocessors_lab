#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <stdio.h>
#include <util/delay.h>


// shared between ISR and main
volatile uint32_t period_us;          // 1 tick = 1 µs at 1 MHz, no prescale
volatile uint8_t  new_period_ready = 0;
volatile uint32_t frequency;

// ---- serial ----
void serial_init(void){
	// Asynchronous, no parity, 1 stop, 8 data bits (URSEL=1)
	UCSRC = 0b10000110;
	UCSRA = 0x00;          // normal speed
	// 1200 bps @ 1 MHz -> 0x33
	UBRRL = 0x33;
	UBRRH = 0x00;
	// enable RX and TX, interrupts off
	UCSRB = 0b00011000;
}

void serial_send(unsigned char data){
	while ((UCSRA & (1 << UDRE)) == 0) {;}
	UDR = data;
}

unsigned char serial_receive(void){
	while ((UCSRA & (1 << RXC)) == 0) {;}
	return UDR;
}

int main(void) {
	DDRD = 0b00100000; // set port D for output (D.5 is OC1A)
	// Set register TCCR1A
	// WGM11:WGM10 = 10: with WGM13-WGM12 to select timer mode 1110
	// Fast PWM, timer 1 runs from 0 to ICR1
	// COM1A1:COM1A0 = 10: clear OC1A when compare match, set OC1A when 0
	// compare match occurs when timer = OCR1A
	
	TCCR1A = 0b10000010;
	// Set register TCCR1B
	// WGM13:WGM12 = 11
	// CS12:CS0 = 001: internal clock 1MHz, no prescaler
	
	TCCR1B = 0b00011001;
	//ICR1 = 1000; // period of output signal
	OCR1A = 250; // pulse width of output signal
	
	stdout = fdevopen(serial_send, NULL);
	stdin = fdevopen(NULL, serial_receive);
	serial_init();
	
	unsigned char a, b, c;
	int a_int, b_int;
		
	while(1) {
		 ICR1 = 3030; // E4 (330 Hz)
		 _delay_ms(250);

		 ICR1 = 3210; // D#4 (311 Hz)
		 _delay_ms(250);

		 ICR1 = 3030; // E4 (330 Hz)
		 _delay_ms(250);

		 ICR1 = 3210; // D#4 (311 Hz)
		 _delay_ms(250);

		 ICR1 = 3030; // E4 (330 Hz)
		 _delay_ms(250);

		 ICR1 = 4050; // B3 (247 Hz)
		 _delay_ms(250);

		 ICR1 = 3400; // D4 (294 Hz)
		 _delay_ms(250);

		 ICR1 = 3820; // C4 (262 Hz)
		 _delay_ms(250);

		 ICR1 = 4550; // A3 (220 Hz)
		 _delay_ms(500);

		 ICR1 = 28;   // silence (36k Hz)
		 _delay_ms(250);

		 ICR1 = 7630; // C3 (131 Hz)
		 _delay_ms(250);

		 ICR1 = 6060; // E3 (165 Hz)
		 _delay_ms(250);

		 ICR1 = 4550; // A3 (220 Hz)
		 _delay_ms(250);

		 ICR1 = 4050; // B3 (247 Hz)
		 _delay_ms(500);

		 ICR1 = 28;   // silence (36k Hz)
		 _delay_ms(250);

		 ICR1 = 6060; // E3 (165 Hz)
		 _delay_ms(250);

		 ICR1 = 4810; // G#3 (208 Hz)
		 _delay_ms(250);

		 ICR1 = 4050; // B3 (247 Hz)
		 _delay_ms(250);

		 ICR1 = 3820; // C4 (262 Hz)
		 _delay_ms(500);

	}
	
	
	
		
	
	
	return 0;
}
