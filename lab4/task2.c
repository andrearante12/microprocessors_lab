#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <stdio.h>

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
	ICR1 = 1000; // period of output signal
	OCR1A = 250; // pulse width of output signal
	
	stdout = fdevopen(serial_send, NULL);
	stdin = fdevopen(NULL, serial_receive);
	serial_init();
	
	unsigned char a, b, c;
	int a_int, b_int;
		
	while (1) {
			
		printf("Period = ");

		if (scanf("%c", &a) == 1) {   // check input success
			a_int = (int) a - 48;
		}
		
		scanf("%c", &c);
		
		printf("\n\Pulse Width = ");
		if (scanf("%c", &b) == 1) {   // check input success
			b_int = (int) b - 48;
		}
		
		ICR1 = a_int; // period of output signal
		OCR1A = b_int; // pulse width of output signal
		
		
	}
	
	return 0;
}
