#define F_CPU 1000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdbool.h>

/* -------- UART 1200 @ 1 MHz (U2X=1) -------- */
#define BAUD    1200UL
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
	UCSRC = (1<<URSEL) | (1<<UCSZ1) | (1<<UCSZ0); // 8N1
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

/* -------- Comparator Init (PD6=Ain0, PD7=Ain1) -------- */
static void comparator_init(void){
	/* PD6 (AIN0) input, PD7 (AIN1) input */
	DDRD &= ~((1<<PD6) | (1<<PD7));

	/* Comparator ON, no interrupt, normal mode
	   ACSR = 0x00;
	   Bit 5 ACO gives comparator output
	*/
	ACSR = 0x00;
}

int main(void){
	uart_init();
	comparator_init();

	while(1){

		/* Read comparator output */
		uint8_t ac = ACSR & (1<<ACO);
		
		// AIN0 is PD6
		// AIN1 is PD7

		if(ac){
			// AIN0 > AIN1
			uart_puts("AIN0 > AIN1\n");
		} else {
			// AIN0 < AIN1
			uart_puts("AIN0 < AIN1\n");
		}

		_delay_ms(100);  // print every 100 ms
	}
}



