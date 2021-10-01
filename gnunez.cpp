//Guadalupe Nunez
//September 20,2021
//Lab 06

#include "fonts.h"

void show_gnunez_credits(int x, int y)
{

    //add walk2 code
    Rect r;
    r.bot = y;
    r.left = x;
    r.center = 1;
    ggprint8b(&r, 16, 0x00ffff44, "Lupe created credits");

}

