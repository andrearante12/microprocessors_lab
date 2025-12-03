#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdbool.h>

/* UART Setup */
#define BAUD 1200UL
#define USE_U2X 1
#if USE_U2X
#define UBRR_VAL ((F_CPU/(8*BAUD))-1)
#else
#define UBRR_VAL ((F_CPU/(16*BAUD))-1)
#endif

static void uart_init(void){
	UBRRH = (uint8_t)(UBRR_VAL >> 8);
	UBRRL = (uint8_t)(UBRR_VAL & 0xFF);
	#if USE_U2X
	UCSRA = (1<<U2X);
	#else
	UCSRA = 0;
	#endif
	UCSRB = (1<<RXEN) | (1<<TXEN);
	UCSRC = (1<<URSEL) | (1<<UCSZ1) | (1<<UCSZ0);
}

static void uart_putc(char c){
	while(!(UCSRA & (1<<UDRE))){}
	UDR = c;
}

static void uart_puts(const char* s){
	while(*s){
		if(*s=='\n') uart_putc('\r');
		uart_putc(*s++);
	}
}

/* Comparator Initialization */
static void comparator_init(void){
	DDRB &= ~((1<<PB2) | (1<<PB3)); // Ain0, Ain1 inputs
	PORTB &= ~((1<<PB2) | (1<<PB3)); // no pullups
	ACSR = 0x00; // Comparator enabled
}

int main(void){
	uart_init();
	comparator_init();
	
	DDRD |= (1<<PD5); // PD5 = OC1A PWM output
	TCCR1A = (1<<COM1A1) | (1<<WGM11);  // Fast PWM, ICR1 top
	TCCR1B = (1<<WGM13) | (1<<WGM12) | (1<<CS10); // No prescaler
	ICR1 = 1000; // PWM top ? controls output resolution
	
	float low = 0.0;
	float high = 5.0;
	float mid = 0.0;

	char buf[16];

	while(1){
		mid = (low + high) / 2.0;

		// Convert mid voltage (0-5V) into PWM duty (0-1000)
		uint16_t duty = (uint16_t)((mid / 5.0) * 1000.0);
		OCR1A = duty;

		_delay_ms(1000); // allow voltage to settle before reading

		uint8_t ac = ACSR & (1<<ACO);
		
		if(ac){
			uart_puts("Ref > Input\n");
			high = mid;
			} else {
			uart_puts("Ref < Input\n");
			low = mid;
		}

		dtostrf(mid, 1, 3, buf);
		uart_puts("Mid = ");
		uart_puts(buf);
		uart_puts("\n");

		if((high - low) < 0.0000005){
			uart_puts("LOCKED VALUE: ");
			uart_puts(buf);
			uart_puts(" V\n\n");
			low = 0;
			high = 5.0;
			_delay_ms(1000);
		}

		_delay_ms(100);
	}
}
