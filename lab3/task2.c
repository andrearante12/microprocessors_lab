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

// ---- timer1 input capture ISR ----
ISR(TIMER1_CAPT_vect){
	// grab measured period and reset timer, exactly like lecture code
	period_us = ICR1;
	TCNT1 = 0;
	new_period_ready = 1;      // tell main() to print/display
}

int main(void) {
	// LEDs on PORTB
	DDRB = 0xFF;
	// keypad/others on A – leave as input
	DDRA = 0x00;
	// ICP1 (PE0) must be input – feed function generator here
	DDRE &= ~(1 << PE0);

	serial_init();

	// hook stdio to our UART once
	stdout = fdevopen(serial_send, NULL);
	stdin  = fdevopen(NULL, serial_receive);

	// Timer1: normal mode
	TCCR1A = 0x00;
	// noise canceler + capture on rising edge + clk/1 (1 MHz, no prescaler)
	// this is the line from the lecture: 0b11000001
	TCCR1B = (1 << ICNC1) | (1 << ICES1) | (1 << CS10);   // = 0xC1

	// enable Timer1 input capture interrupt
	TIMSK = (1 << TICIE1);   // same as 0b00100000 on this part

	sei();  // global interrupt enable

	while (1) {
		if (new_period_ready) {
			uint16_t p = period_us;
			new_period_ready = 0;

			// show high 8 bits on LEDs (active low on STK500)
			PORTB = ~(p >> 8);

			// print period; you can also compute frequency below
			frequency = 1000000/p;
			printf("Signal period = %u us, signal frequency = %u Hz \r\n", p, frequency);
			
			//printf("Frequency: %u \r\n", frequency);

			// if you want frequency too:
			// uint32_t f = 1000000UL / p;
			// printf("Freq = %" PRIu32 " Hz\r\n", f);
		}
	}
}
