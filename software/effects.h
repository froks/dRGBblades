#ifndef EFFECTS_H_
#define EFFECTS_H_

#include <stdint.h>

void allpixel_randcolor(uint16_t duration);
void colorcycle_rainbow(uint16_t duration);
void colorcycle_pong(uint16_t duration);
void colorcycle_fillremove(uint16_t duration);
void colorcycle_all(uint16_t duration);
void singlecolor_edgemiddle(uint16_t duration, uint8_t r, uint8_t g, uint8_t b);
void singlecolor_kitt(uint16_t duration);
void singlecolor_wave(uint16_t duration);
void singlecolor_all(uint16_t duration, uint8_t r, uint8_t g, uint8_t b);

void low_battery_lights(uint8_t r, uint8_t g, uint8_t b);


#endif /* EFFECTS_H_ */