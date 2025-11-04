#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint16_t overflow_count = 0;
volatile uint8_t led_state = 0; // 0 = odd LEDs, 1 = even LEDs

void delay(int n) {
	if (overflow_count >= 16*n) {
		overflow_count = 0;
		
		if (led_state == 0) {
			// Turn on odd LEDs (bits 1,3,5,7), turn off even LEDs
			PORTB = 0b10101010;  // 0xAA
			led_state = 1;
			} else {
			// Turn on even LEDs (bits 0,2,4,6), turn off odd LEDs
			PORTB = 0b01010101;  // 0x55
			led_state = 0;
		}
	}
}

ISR(TIMER1_OVF_vect) {
	overflow_count++;
	delay(4);
	
}

int main(void) {
	DDRB = 0xFF;        // Set PORTB as output
	PORTB = 0x00;       // Start with all LEDs off
	
	// Timer1 configuration - using prescaler for better timing
	TCCR1A = 0x00;      // Normal mode
	TCCR1B = (1<<CS10); // 256 prescaler (0b00000100)
	
	TIMSK = (1 << TOIE1); // Enable Timer1 overflow interrupt
	sei();              // Enable global interrupts
	
	while (1) {
		// Main loop - everything happens in ISR
	}
}

