#include "init.h"
#include "effects.h"
#include "timer.h"

// Fuses extended 0xFF
// high 0xDF
// low 0x62

// CLK Output = PA2
// Data Output =  PA7

int main(void)
{
	init_all();
	if (is_top()) 
	{
		simple_stripe(10, 127, 127, 127);
		rainbowCycle(0);
	}
	else
	{
		simple_stripe(10, 127, 0, 127);
		while (true)
		{
			wave(10);
			rainbowCycle(10);
		}
	}
}

