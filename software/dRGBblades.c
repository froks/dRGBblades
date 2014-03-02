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
		singlecolor_edgemiddle(60, 0x7F007F, 0x7F007F, 0x7F007F);
		while (true)
		{
			colorcycle_rainbow(30);
			colorcycle_pong(20);
			colorcycle_fillremove(40);
			colorcycle_wave(30);
		}
	}
	else
	{
		singlecolor_edgemiddle(60, 0x7F, 0x7F, 0x7F);
		while (true)
		{
			singlecolor_wave(60, 0, 0, 127);
			singlecolor_edgemiddle(20, 0x7F, 0x7F00, 0x7F0000);
		}
	}
}

