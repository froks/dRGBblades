#include "init.h"
#include "lpd8806.h"
#include "timer.h"
#include <avr/io.h>
#include <util/delay.h>
volatile uint8_t side = 0;

void setup_side(void)
{
	// Turn on PB2, Measure Voltage, High = Top, Low = Bottom
	// Deactivate afterwards
	DDRB |= _BV(PB2);
	PORTB |= _BV(PB2);
	_delay_us(10);
	if ((PINB & _BV(PB2)) > 0)
	{
		// top
		side = 0;
	}
	else
	{
		// bottom
		side = 1;
	}
	DDRB &= ~(_BV(PB2));
	PORTB &= ~(_BV(PB2));
}

inline bool is_top(void)
{
	return side == 0;
}

void init_all(void)
{
	_delay_ms(20);
	CLKPR = _BV(CLKPCE); // Remove CLKDIV8 flag
	CLKPR = 0x0;
	_delay_ms(10);
	setup_side();
	setup_timer();
	lpd8806_init();
	lpd8806_set_length(STRIPE_LENGTH);
	lpd8806_start();
}