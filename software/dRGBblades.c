/*
 * dRGBblades.c
 *
 * Created: 24.08.2013 14:14:21
 *  Author: flo
 */ 

#include <avr/io.h>

#include "lpd8806.h"

// CLK Output = PA2
// Data Output =  PA7

#define STRIPE_LENGTH 20

void check_adc(void);

volatile uint8_t undervoltage = 0;
volatile uint8_t side = 0;

// Adapted from LPD8806-Arduino library

/* Helper functions */

// Convert separate R,G,B into combined 32-bit GRB color:
uint32_t Color(uint8_t r, uint8_t g, uint8_t b) {
  return ((uint32_t)(r | 0x80) << 16) |
         ((uint32_t)(g | 0x80) <<  8) |
                     b | 0x80 ;
}

//Input a value 0 to 384 to get a color value.
//The colours are a transition r - g - b - back to r

uint32_t Wheel(uint16_t WheelPos)
{
  uint8_t r = 0, g = 0, b = 0;
  switch(WheelPos / 128)
  {
    case 0:
      r = 127 - WheelPos % 128; // red down
      g = WheelPos % 128;       // green up
      b = 0;                    // blue off
      break;
    case 1:
      g = 127 - WheelPos % 128; // green down
      b = WheelPos % 128;       // blue up
      r = 0;                    // red off
      break;
    case 2:
      b = 127 - WheelPos % 128; // blue down
      r = WheelPos % 128;       // red up
      g = 0;                    // green off
      break;
  }
  return(Color(r, g, b));
}

void rainbowCycle(void) {
  uint16_t i, j;

  for (j=0; j < 384 * 5; j++) {     // 5 cycles of all 384 colors in the wheel
    for (i = 0; i < STRIPE_LENGTH; i++) {
      // tricky math! we use each pixel as a fraction of the full 384-color
      // wheel (thats the i / strip.numPixels() part)
      // Then add in j which makes the colors go around per pixel
      // the % 384 is to make the wheel cycle around
	  lpd8806_set_pixel_rgb(STRIPE_LENGTH - i, Wheel(((i * 384 / STRIPE_LENGTH) + j) % 384));
    }
	check_adc();
    lpd8806_update_strip();
  }
}

void setup_adc(void)
{
	ADMUX = _BV(MUX0) | _BV(MUX1); // Use Vcc as Voltage reference, PA3 single ended input
	
	// ADLAR - left adjust the result
//	ADCSRB = (1 << ADLAR);

	// ADCN (ADC Enable), ADSC (ADC Start Conversion), Prescale Clock / 64
	// _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0); 
	ADCSRA = _BV(ADEN) | _BV(ADSC) | _BV(ADATE) | _BV(ADPS2) | _BV(ADPS0); 
}

uint32_t adcinavg = 0;
uint32_t adcincount = 0;

void check_adc(void)
{
	uint16_t adcin = ((ADCH << 8) | ADCL) & 0x3FF;
	// 0-1024 = 0 - 5V1; adcvalue = (vin * 1024) / 5.1
	adcinavg += adcin;
	adcincount++;
	
	if (adcincount == 10)
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
		while(1)
		{
			for (uint8_t i = 0; i < STRIPE_LENGTH; ++i)
			{
				lpd8806_set_pixel(i, 0, 0, 0);
			}
			uint8_t r, g, b;
			if (side == 0)
			{
				r = 127;
				g = 0;
				b = 0;
			}
			else
			{
				r = 0;
				g = 127;
				b = 0;
			}
			lpd8806_set_pixel(0, r, g, b);
			lpd8806_set_pixel(STRIPE_LENGTH - 1, r, g, b);
			lpd8806_set_pixel(STRIPE_LENGTH >> 1, r, g, b);
			lpd8806_update_strip();
		}
	}
}

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

#define PI 3.14159265
void wave() {
	const uint8_t sinTable[STRIPE_LENGTH] = { 63, 43, 24, 10, 2, 0, 5, 17, 33, 53, 74, 94, 110, 122, 127, 125, 117, 103, 84, 63 }; 
	uint8_t offset = 0;
	while (1)
	{
		for(uint16_t i = 0; i < STRIPE_LENGTH; ++i) {
			uint8_t c = sinTable[(offset + i) % STRIPE_LENGTH];
			lpd8806_set_pixel((STRIPE_LENGTH - i), 0, 0, c);
		}		
		check_adc();
		lpd8806_update_strip();
		++offset;
		offset = offset % STRIPE_LENGTH;
		_delay_ms(20);
	}
}	

int main(void)
{
	_delay_ms(100);
	CLKPR = _BV(CLKPCE); // Remove CLKDIV8 flag
	CLKPR = 0x0;
	_delay_ms(10);
	setup_side();
	setup_adc();
	lpd8806_init();
	lpd8806_set_length(STRIPE_LENGTH);
	lpd8806_start();
	check_adc();
	
	if (side == 0)
	{
		while(1) {
			rainbowCycle();
		}		
	}
	else
	{
		while(1) {
			wave();
		}		
	}
}
