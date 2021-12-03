// Author: Andrea Almanza
// FILE: globals.h
// FALL 2021
// 
#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <GL/glx.h>
#include <stdlib.h>
#include <vector>

#include "fonts.h"

typedef double Flt;
typedef double Vec[3];
typedef Flt Matrix[4][4];
typedef struct t_resolution {
    int xres;
    int yres;
} XResolution;
#define MAXBUTTONS 3
typedef struct t_button {
    Rect r;
    void (*onClick)();
    char text[32];
    int id;
    int over;   // cursor is over the button
    int down;   // cursor is holding down on the button
    int click;  // cursor has stopped holding down inside button perimeter, registering as a button click
    GLfloat color[4];
    GLfloat dcolor[4];
    unsigned int text_color;
} Button;
Button button[MAXBUTTONS];
typedef struct t_butt_options {
    char text[32];
    float x; //between 0 and 1
    float y; //between 0 and 1 
    int width;
    int height;
    bool highlight;
    unsigned int text_color;
    int groupId;
    void (*onClick)();
} ButtonOptions;

typedef struct t_mouse {
    int lbutton;
    int rbutton;
    int x;
    int y;
} Mouse;

class Image {
    public:
        int width, height;
    unsigned char *data;
    ~Image();
    Image(const char *);

};
typedef struct t_options_background {

    float color[4];

    const char *imagePath;

    XResolution res;

} BackgroundOptions;
/*class Menu {
private:
    std::vector<Button>& buttons;
    Mouse *mouse;
    float *color;
    GLuint textureId;
    int hasImage;
    void checkButtons();
public:
    int numButtons;
    bool active;
    Rect rect;
    Menu(std::vector<Button>&, Mouse*, BackgroundOptions);
    void render();
};*/


class X11_wrapper {
private:
    Display *dpy;
    Window win;
    XResolution res;
    char windowTitle[40];
    void (*onWindowResize)(XResolution);
public:
    X11_wrapper(GLint[], XResolution, long, const char[], void (*)(XResolution));
    ~X11_wrapper();
    void setTitle(const char[]);
    void setupScreenRes(const int, const int);
    void reshapeWindow(const int, const int);
    void checkResize(XEvent*);
    bool getXPending();
    XEvent getXNextEvent();
    void swapBuffers();
};
class Sprite {
public:
    int onoff;
    int frame;
    double delay;
    Vec pos; 
    Image *image;
    GLuint tex; 
    struct timespec time;
    Sprite() {
        onoff = 0; 
        frame = 0; 
        image = NULL;
        delay = 0.1; 
    }    
};

class Global {
public:
    unsigned char keys[65536];
    int xres, yres;
    int movie, movieStep;
    int walk;
    int walkFrame;
    double delay;
    int mainMenu;
    int credits;
    int settings;
    int show_credits;
    int score;
    Image *walkImage;
    GLuint walkTexture;
    GLuint animalEleTexture;
    Vec box[20];
    Sprite exp;
    Sprite exp44;
    Vec ball_pos;
    Vec ball_vel;
    //camera is centered at (0,0) lower-left of screen. 
    Flt camera[2];
    Mouse mouse;
    ~Global();
    Global();
};

#endif
