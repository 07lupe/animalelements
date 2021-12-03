//3350
//Author:   Mario Ervin
//Date:     9/30/2021


#include "fonts.h"
#include <cstring>
#include <cstdio>
#include <cstddef>
#include <stdlib.h>
#include <unistd.h>
#include <GL/glx.h>


//
//
void show_mervin_credits(int x, int y)
{
    Rect r;
    r.bot = y;
    r.left = x;
    r.center = 1;
    ggprint16(&r, 16, 0x00fff000, "Mario Ervin");
}
