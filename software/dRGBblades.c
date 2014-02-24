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
		allpixel_randcolor(10);
		colorcycle_rainbow(10);
		colorcycle_pong(10);
		colorcycle_fillremove(10);
		colorcycle_all(10);
		colorcycle_wave(10);
		singlecolor_edgemiddle(10, 0xFF, 0xFF, 0xFF);
		singlecolor_kitt(10, 127, 0, 0);
		singlecolor_wave(10, 0, 0, 127);
		singlecolor_all(10, 127, 0, 127);
		allpixel_colorwave(10);
	}
	else
	{
		while (true)
		{
			allpixel_colorwave(10); 
//			allpixel_randcolor(20);
		}
	}
}

