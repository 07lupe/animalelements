#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>

#include "fonts.h"

#define rnd() (double)rand()/(double)RAND_MAX
//prototypes
void init();
void init_opengl();
void render();
void get_grid_center(const int g, const int i, const int j, int cent[2]);
int xres=1200;
int yres=800;

#define MAXBUTTONS 8
typedef struct t_button {
    Rect r;
    char text[32];
    int over;
    int down;
    int click;
    float color[3];
    float dcolor[3];
    unsigned int text_color;
} Button;
Button button[MAXBUTTONS];
int nbuttons=0;
void check_mouse(XEvent *e);
void init(void)
{
    board_dim = 400;
    //quad upper-left corner
    //bq is the width of one grid section
    int bq = (board_dim / grid_dim);
    qsize = bq / 2 - 1; 
    //
    //initialize buttons...
    nbuttons=0;
    //
    //Quit button
    //size and position
    button[nbuttons].r.width = 200;
    button[nbuttons].r.height = 50;
    button[nbuttons].r.left = 10;
    button[nbuttons].r.bot = 10;
    button[nbuttons].r.right =
        button[nbuttons].r.left + button[nbuttons].r.width;
    button[nbuttons].r.top = button[nbuttons].r.bot + button[nbuttons].r.height;
    button[nbuttons].r.centerx =
        (button[nbuttons].r.left + button[nbuttons].r.right) / 2;
    button[nbuttons].r.centery = 
        (button[nbuttons].r.bot + button[nbuttons].r.top) / 2;
    strcpy(button[nbuttons].text, "Quit");
    button[nbuttons].down = 0;
    button[nbuttons].click = 0;
    button[nbuttons].color[0] = 0.4f;
    button[nbuttons].color[1] = 0.4f;
    button[nbuttons].color[2] = 0.7f;
    button[nbuttons].dcolor[0] = button[nbuttons].color[0] * 0.5f;
    button[nbuttons].dcolor[1] = button[nbuttons].color[1] * 0.5f;
    button[nbuttons].dcolor[2] = button[nbuttons].color[2] * 0.5f;
    button[nbuttons].text_color = 0x00ffffff;
    nbuttons++;
    //
    //Reset button
    //size and position
    button[nbuttons].r.width = 200;
    button[nbuttons].r.height = 100;
    button[nbuttons].r.left = xres/2 - button[nbuttons].r.width/2;
    button[nbuttons].r.bot = 50;
    button[nbuttons].r.right =
        button[nbuttons].r.left + button[nbuttons].r.width;
    button[nbuttons].r.top = button[nbuttons].r.bot + button[nbuttons].r.height;
    button[nbuttons].r.centerx =
        (button[nbuttons].r.left + button[nbuttons].r.right) / 2;
    button[nbuttons].r.centery = 
        (button[nbuttons].r.bot + button[nbuttons].r.top) / 2;
    strcpy(button[nbuttons].text, "New Game");
    button[nbuttons].down = 0;
    button[nbuttons].click = 0;
    button[nbuttons].color[0] = 0.4f;
button[nbuttons].color[1] = 0.4f;
    button[nbuttons].color[2] = 0.7f;
    button[nbuttons].dcolor[0] = button[nbuttons].color[0] * 0.5f;
    button[nbuttons].dcolor[1] = button[nbuttons].color[1] * 0.5f;
    button[nbuttons].dcolor[2] = button[nbuttons].color[2] * 0.5f;
    button[nbuttons].text_color = 0x00ffffff;
    nbuttons++;

     button[nbuttons].r.width = 200;
    button[nbuttons].r.height = 100;
    button[nbuttons].r.left = xres/2 - button[nbuttons].r.width/2;
    button[nbuttons].r.bot = 50;
    button[nbuttons].r.right =
        button[nbuttons].r.left + button[nbuttons].r.width;
    button[nbuttons].r.top = button[nbuttons].r.bot + button[nbuttons].r.height;
    button[nbuttons].r.centerx =
        (button[nbuttons].r.left + button[nbuttons].r.right) / 2;
    button[nbuttons].r.centery =
        (button[nbuttons].r.bot + button[nbuttons].r.top) / 2;
    strcpy(button[nbuttons].text, "Show Credits");
    button[nbuttons].down = 0;
    button[nbuttons].click = 0;
    button[nbuttons].color[0] = 0.4f;
    button[nbuttons].color[1] = 0.4f;
    button[nbuttons].color[2] = 0.7f;
    button[nbuttons].dcolor[0] = button[nbuttons].color[0] * 0.5f;
    button[nbuttons].dcolor[1] = button[nbuttons].color[1] * 0.5f;
    button[nbuttons].dcolor[2] = button[nbuttons].color[2] * 0.5f;
    button[nbuttons].text_color = 0x00ffffff;
    nbuttons++;

}
void mouse_click(int ibutton, int action, int x, int y)
{
    int con;
    if (action == 1) {
        int i,j,k,m=0;
        //center of a grid
        int cent[2];
        //
        //buttons?
        for (i=0; i<nbuttons; i++) {
            if (button[i].over) {
                button[i].down = 1;
                button[i].click = 1;
                if (i==0) {
                    //user clicked QUIT
                    done = 1;
                }
                if (i==1) {
                    //user clicked button 0
                    reset_grids();
                }
            }
        }
        for (i=0; i<grid_dim; i++) {
            for (j=0; j<grid_dim; j++) {
                if (gamemode == MODE_PLACE_SHIPS) {
                    get_grid_center(1,i,j,cent);
                    if (x >= cent[0]-qsize &&
                        x <= cent[0]+qsize &&
                        y >= cent[1]-qsize &&
                        y <= cent[1]+qsize) {
                        if (ibutton == 1) {
                            //does this quad have any connecting quads?
                            con = check_connecting_quad(i,j,1);
                            if (con != 0) {
                                //same ship continued
                                grid1[i][j].status=1;
                                grid1[i][j].shipno=nships+1;
                            } else {
                                if (nships < MAXSHIPS) {
                                    //new ship being placed!
                                    grid1[i][j].status = 1;
                                    nships++;
                                    grid1[i][j].shipno = nships+1;
                                }
                            }
                        }
                        //if (ibutton == GLFW_MOUSE_BUTTON_RIGHT)
                        //grid1[i][j].status=2;
                        m=1;
                        break;
                    }
                }
 if (gamemode == MODE_FIND_SHIPS) {
                    get_grid_center(2,i,j,cent);
                    if (x >= cent[0]-qsize &&
                        x <= cent[0]+qsize &&
                        y >= cent[1]-qsize &&
                        y <= cent[1]+qsize) {
                        if (ibutton == 1) {
                            nbombs--;
                            if (grid1[i][j].status) {
                                int s = grid1[i][j].shipno;
                                grid2[i][j].status = 2;
                                {
                                    //is this ship sunk?
                                    if (check_for_sink(s)) {
                                        nshipssunk++;
                                        nbombs += 5;
                                        if (nshipssunk >= nships) {
                                            gamemode = MODE_GAMEOVER;
                                        }
                                    }
                                }
                            }
                            if (nbombs <= 0) {
                                gamemode = MODE_GAMEOVER;
                            }
                        }
                        //if (ibutton == 2)
                        //  grid2[i][j].status=2;
                        m=1;
                        break;
                    }
                }
            }
            if (m) break;
        }
    }
    //
    if (action == 2) {
        int i;
        for (i=0; i<nbuttons; i++) {
            button[i].down = 0;
            button[i].click = 0;
        }
    }
}


void show_andreas_credits(int x, int y) {
    Rect r;
    r.bot = y;
    r.left = x;
    r.center = 1;
    ggprint8b(&r, 16, 0x00ffff44, "Andrea did something");
}

