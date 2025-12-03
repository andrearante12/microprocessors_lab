#include <avr\io.h>
int main(void){
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
	ICR1 = 1000;
	OCR1A = 500;
	// period of output signal
	// pulse width of output signal
	while(1){;}
	return 0;
}
