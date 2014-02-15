#ifndef EFFECTS_H_
#define EFFECTS_H_

#include <stdint.h>

void rainbowCycle(uint16_t duration);
void wave(uint16_t duration);
void simple_stripe(uint16_t duration, uint8_t r, uint8_t g, uint8_t b);

void low_battery_lights(uint8_t r, uint8_t g, uint8_t b);


#endif /* EFFECTS_H_ */