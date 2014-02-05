/*
 * lpd8806.h
 *
 * Created: 24.08.2013 18:25:53
 *  Author: flo
 */ 


#ifndef LPD8806_H_
#define LPD8806_H_

#include <inttypes.h>
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <alloca.h>

void lpd8806_init(void);

void lpd8806_set_length(uint16_t n);
uint16_t lpd8806_get_length(void);
void lpd8806_set_pixel(uint16_t n, uint8_t r, uint8_t g, uint8_t b);
void lpd8806_set_pixel_grb(uint16_t n, uint32_t c);
void lpd8806_start(void);
void lpd8806_update_strip(void);

#endif /* LPD8806_H_ */