/*
 * lpd8806.c
 *
 * Created: 24.08.2013 18:24:48
 *  Author: flo
 */ 

#include "lpd8806.h"

uint8_t *pixels;
uint16_t num_leds;

uint16_t num_bytes;

#define DATAPIN (1 << PA7)
#define CLKPIN (1 << PA2)

// Adapted from LPD8806-Arduino library

void lpd8806_init(void)
{
	// PA2 = CLK, PA7 = DATA
	DDRA = CLKPIN | DATAPIN; // USCK / MOSI = OUTPUT
}

void lpd8806_set_length(uint16_t n)
{
	if (pixels != NULL)
	{
		free(pixels);
	}
	
	num_leds = n;
	uint16_t latch_bytes = ((num_leds + 31) / 32);
	num_bytes = (num_leds * 3) + latch_bytes;
	if (NULL != (pixels = malloc(num_bytes)))
	{
		memset(pixels, 0x80, num_leds * 3);
		memset(&pixels[num_leds * 3], 0, latch_bytes);
	}
	else
	{
		num_bytes = 0;
		num_leds = 0;
	}
}

void lpd8806_start(void)
{
	PORTA &= ~DATAPIN;
	uint8_t i;
	for (i = ((num_leds+31)/32)*8; i > 0; --i) {
      PORTA |=  CLKPIN;
      PORTA &= ~CLKPIN;
    }	
	
	for (i = num_leds; i > 0; --i)
	{
		lpd8806_set_pixel(i, 0, 0, 0);
	}
	lpd8806_update_strip();
}

uint16_t lpd8806_get_length(void)
{
	return num_leds;
}

// colors 0-127
void lpd8806_set_pixel(uint16_t n, uint8_t r, uint8_t g, uint8_t b)
{
	if (n >= num_leds)
	{
		return;
	}
	
	uint8_t *ptr = &pixels[n * 3];
	*ptr++ = r | 0x80;
	*ptr++ = g | 0x80;
	*ptr++ = b | 0x80;
}

void lpd8806_set_pixel_rgb(uint16_t n, uint32_t c)
{
	if (n >= num_leds)
	{
		return;
	}
	
	uint8_t *ptr = &pixels[n * 3];
	*ptr++ = (c >> 16) | 0x80;
	*ptr++ = (c >> 8) | 0x80;
	*ptr++ = (c) | 0x80;
}


void lpd8806_update_strip(void)
{
	uint16_t i = num_bytes;
	uint8_t *ptr = pixels;
	
	while (i--)
	{
		// Data = PA7
		// Clock = PA2
		uint8_t data = *ptr++;
		for (uint8_t bit = 0x80; bit; bit >>= 1)
		{
			// Set Data Pin
			if (data & bit) {
				PORTA |= DATAPIN;
			} else {
				PORTA &= ~DATAPIN;
			}
			// Toggle clock
			PORTA |= CLKPIN;
			PORTA &= ~CLKPIN;
		}
	}
}
