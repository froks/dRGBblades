#include "init.h"
#include "effects.h"
#include "timer.h"

#include <util/delay.h>

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
		colorcycle_rainbow(10);
		singlecolor_kitt(10);
		colorcycle_pong(10);
		singlecolor_wave(10);
		singlecolor_all(10, 127, 0, 127);
		singlecolor_edgemiddle(10, 127, 127, 127);
		colorcycle_rainbow(0);
	}
	else
	{
		while (true)
		{
			colorcycle_all(20);
			colorcycle_fillremove(20);
		}
	}
}

