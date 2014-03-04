#include "lpd8806.h"

uint8_t *pixels = NULL;
uint8_t *effectswitchpixels = NULL;
volatile uint8_t effectchangeactive = 0;
volatile uint8_t alphablend = 0;

uint16_t num_leds;
uint16_t num_bytes;
uint16_t num_latchbytes;

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
	if (pixels != NULL)	{
		free(pixels);
	}
    if (effectswitchpixels != NULL) {
        free(effectswitchpixels);
    }
    
	
	num_leds = n;
	num_latchbytes = ((num_leds + 31) / 32);
	num_bytes = (num_leds * 3) + num_latchbytes;
	if (NULL != (pixels = malloc(num_bytes)))
	{
		memset(pixels, 0x80, num_leds * 3);
		memset(&pixels[num_leds * 3], 0, num_latchbytes);
	}
	else
	{
		num_bytes = 0;
		num_leds = 0;
	}
	
    if (NULL != (effectswitchpixels = malloc(num_bytes)))
	{
    	memset(effectswitchpixels, 0x80, num_leds * 3);
    	memset(&effectswitchpixels[num_leds * 3], 0, num_latchbytes);
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
	if (n >= num_leds) {
		return;
	}
	
	uint8_t *ptr = &pixels[n * 3];
	*ptr++ = r | 0x80;
	*ptr++ = g | 0x80;
	*ptr++ = b | 0x80;
}

void lpd8806_set_pixel_rgb(uint16_t n, uint32_t c)
{
	if (n >= num_leds) {
		return;
	}
	
	uint8_t *ptr = &pixels[n * 3];
	*ptr++ = (c >> 16) | 0x80;
	*ptr++ = (c >> 8) | 0x80;
	*ptr++ = (c) | 0x80;
}


void lpd8806_update_strip(void)
{
	for (uint8_t i = 0; i < num_bytes; ++i)	{
		// Data = PA7
		// Clock = PA2
		uint8_t data = *&pixels[i];
        if (effectchangeactive == 1 && alphablend > 0 && i < (num_bytes - num_latchbytes)) {
    		uint16_t dataeffectchange = (*&effectswitchpixels[i]) & 0x7F;
            uint16_t v = data & 0x7F;
            v *= (128 - alphablend);
            v += (dataeffectchange * alphablend);
            v /= 128;
            data = 0x80 | v; 
        }
        
		for (uint8_t bit = 0x80; bit; bit >>= 1) {
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

void lpd8806_starteffect(void)
{
/*    if (effectchangeactive == 1) {
        // TODO 
    } */
    memcpy(effectswitchpixels, pixels, num_bytes);
    effectchangeactive = 1;
    alphablend = 128;
}

void lpd8806_effects_isr()
{
    if (effectchangeactive == 1) {
        if (alphablend > 3) {
            alphablend -= 3;
        } else {
            alphablend = 0;
            effectchangeactive = 0;
        }
    }        
}
