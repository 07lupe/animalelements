// Daniel Macias
//
//
#include "fonts.h"


void show_dmacias_credits(int x,int y)
{
	Rect r;
	r.bot = y - 20 ;
	r.left = x;
	r.center = 1;
	ggprint8b(&r, 16,0x00fff000, "Daniel did some programing");

}
