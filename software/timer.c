#include "timer.h"
#include "init.h"
#include "effects.h"

#include <avr/interrupt.h>

volatile uint16_t seconds_since_start = 0;
volatile uint8_t undervoltage = 0;
volatile uint8_t timer_overflow_counter = 0;
volatile uint32_t adcinavg = 0;
volatile uint32_t adcincount = 0;


void setup_adc(void)
{
	ADMUX = _BV(MUX0) | _BV(MUX1); // Use Vcc as Voltage reference, PA3 single ended input
	
	// ADLAR - left adjust the result
//	ADCSRB = (1 << ADLAR);

	// ADCN (ADC Enable), ADSC (ADC Start Conversion), Prescale Clock / 64
	// _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0); 
	ADCSRA = _BV(ADEN) | _BV(ADSC) | _BV(ADATE) | _BV(ADPS2) | _BV(ADPS0); 
}

void check_adc(void)
{
	uint16_t adcin = ((ADCH << 8) | ADCL) & 0x3FF;
	// 0-1024 = 0 - 5V1; adcvalue = (vin * 1024) / 5.1
	adcinavg += adcin;
	adcincount++;
	
	if (adcincount >= 10)
	{
		uint32_t avg = (adcinavg / adcincount);
		if (avg < 750)
		{
			undervoltage++;
		}
		adcinavg = 0;
		adcincount = 0;
	}
	
	if (undervoltage > 0)
	{
		cli();
		if (is_top())
		{
			low_battery_lights(127, 0, 0);
		}
		else
		{
			low_battery_lights(0, 127, 0);
		}
	}
}

void setup_timer(void)
{
	setup_adc();
	// Prescale / 1024 (8000000 / 1024 = 7812,5 interrupts per second / 256 = 30 overflows per second
	TCCR0B |= _BV(CS02) | _BV(CS00);
	TIMSK0 |= _BV(TOIE0);
	sei();
}

inline uint16_t elapsed_seconds()
{
	return seconds_since_start;
}

ISR(TIM0_OVF_vect)
{
	if ((timer_overflow_counter % 5) == 0)
	{
		check_adc();
	}
			
	if (++timer_overflow_counter >= 30)  
	{
		timer_overflow_counter = 0;
		++seconds_since_start;
	}
}

