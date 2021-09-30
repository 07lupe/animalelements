#include "fonts.h"

void show_andreas_credits(int x, int y) {
    Rect r;
    r.bot = y;
    r.left = x;
    r.center = 1;
    ggprint8b(&r, 16, 0x00ffff44, "Andrea did something");
}
