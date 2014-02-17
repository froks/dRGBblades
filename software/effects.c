#include "effects.h"
#include "init.h"
#include "lpd8806.h"
#include "timer.h"

#define DELAY_KITT_MS 20
#define DELAY_WAVE_MS 20
#define DELAY_RAINBOWCYCLE_MS 0

// Precalculated sine-table from 0 to 127, in correct stripe-length 
const uint8_t sinTable[STRIPE_LENGTH] = { 63, 43, 24, 10, 2, 0, 5, 17, 33, 53, 74, 94, 110, 122, 127, 125, 117, 103, 84, 63 }; 

// In part adapted from the LPD8806-Arduino library by adafruit (https://github.com/adafruit/LPD8806)

/* Helper functions */

// Convert separate R,G,B into combined 32-bit GRB color:
uint32_t Color(uint8_t r, uint8_t g, uint8_t b) 
{
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

void low_battery_lights(uint8_t r, uint8_t g, uint8_t b)
{
	while (true)
	{
		for (uint8_t i = 0; i < STRIPE_LENGTH; ++i)
		{
			lpd8806_set_pixel(i, 0, 0, 0);
		}
		lpd8806_set_pixel(0, r, g, b);
		lpd8806_set_pixel(STRIPE_LENGTH - 1, r, g, b);
		lpd8806_set_pixel(STRIPE_LENGTH >> 1, r, g, b);
		lpd8806_update_strip();
	}
}

void rainbowCycle(uint16_t duration) 
{
	uint16_t i, j;

	uint16_t start = elapsed_seconds();
	
	if (duration == 0) {
		start = 0;
	}
	
	while (elapsed_seconds() - start < duration)
	{
		for (j=0; j < 384 * 5; j++) 
		{     
			// 5 cycles of all 384 colors in the wheel
			for (i = 0; i < STRIPE_LENGTH; i++) 
			{
				// tricky math! we use each pixel as a fraction of the full 384-color
				// wheel (thats the i / strip.numPixels() part)
				// Then add in j which makes the colors go around per pixel
				// the % 384 is to make the wheel cycle around
				lpd8806_set_pixel_rgb(STRIPE_LENGTH - i, Wheel(((i * 384 / STRIPE_LENGTH) + j) % 384));
			}
			lpd8806_update_strip();
			#if DELAY_RAINBOWCYCLE_MS > 0
			_delay_ms(DELAY_RAINBOWCYCLE_MS);
			#endif
			if (elapsed_seconds() - start >= duration) 
			{
				break;
			}
		}
	}		
}

volatile uint8_t wave_offset = 0;
	
void wave(uint16_t duration) 
{
	uint16_t start = elapsed_seconds();
	
	if (duration == 0) {
		start = 0;
	}
	
	while (elapsed_seconds() - start < duration)
	{
		for(uint8_t i = 0; i < STRIPE_LENGTH; ++i) 
		{
			uint8_t c = sinTable[(wave_offset + i) % STRIPE_LENGTH];
			lpd8806_set_pixel((STRIPE_LENGTH - i - 1), 0, 0, c);
		}		
		lpd8806_update_strip();
		++wave_offset;
		wave_offset = wave_offset % STRIPE_LENGTH;
		_delay_ms(DELAY_WAVE_MS);
	}
}	

void simple_stripe(uint16_t duration, uint8_t r, uint8_t g, uint8_t b)
{
	uint16_t start = elapsed_seconds();
	
	if (duration == 0) {
		start = 0;
	}
	
	while (elapsed_seconds() - start < duration)
	{
		lpd8806_set_pixel(0, r, g, b);
		lpd8806_set_pixel(STRIPE_LENGTH >> 1, r, g, b);
		lpd8806_set_pixel(STRIPE_LENGTH - 2, r, g, b);
		lpd8806_set_pixel(STRIPE_LENGTH - 1, r, g, b);
		lpd8806_update_strip();
	}		
}

void kitt(uint16_t duration)
{
	uint16_t start = elapsed_seconds();
	
	if (duration == 0) {
		start = 0;
	}
	
	while (elapsed_seconds() - start < duration)
	{
		for (uint8_t index = 0; index < STRIPE_LENGTH - 5; ++index)
		{
			for (uint8_t i = 0; i < STRIPE_LENGTH; ++i)
			{
				if (i == index) 
				{
					lpd8806_set_pixel(i, 30, 0, 0);
				}
				else if (i == index + 1) 
				{
					lpd8806_set_pixel(i, 60, 0, 0);
				}
				else if (i == index + 2) 
				{
					lpd8806_set_pixel(i, 127, 0, 0);
				}
				else if (i == index + 3) 
				{
					lpd8806_set_pixel(i, 60, 0, 0);
				}
				else if (i == index + 4) 
				{
					lpd8806_set_pixel(i, 30, 0, 0);
				}
				else
				{
					lpd8806_set_pixel(i, 0, 0, 0);
				}
			}
			lpd8806_update_strip();
			_delay_ms(DELAY_KITT_MS);
		}
		
		for (uint8_t index = STRIPE_LENGTH - 5; index > 0; --index)
		{
			for (uint8_t i = STRIPE_LENGTH - 1; i > 0; --i)
			{
				if (i == index) 
				{
					lpd8806_set_pixel(i, 30, 0, 0);
				}
				else if (i == index + 1) 
				{
					lpd8806_set_pixel(i, 60, 0, 0);
				}
				else if (i == index + 2) 
				{
					lpd8806_set_pixel(i, 127, 0, 0);
				}
				else if (i == index + 3) 
				{
					lpd8806_set_pixel(i, 60, 0, 0);
				}
				else if (i == index + 4) 
				{
					lpd8806_set_pixel(i, 30, 0, 0);
				}
				else
				{
					lpd8806_set_pixel(i, 0, 0, 0);
				}
			}
			lpd8806_update_strip();
			_delay_ms(DELAY_KITT_MS);
		}
	}
}