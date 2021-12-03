// AUTHOR: Andrea Almanza 
// CMPS 3350 - Professor Gordon
// FALL 2021
// This file contains the main functions for the main menu and 
// buttons for the main menu
// ------------------------------------------------------------
// -----------------------------------------------------------
/*** INCLUDE ***/
#include <GL/glx.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <exception>
#include "fonts.h"
#include "log.h"
#include "globals.h"

/** GLOBAL **/
extern Global gl;

/*** MACROS ***/
#define BUTTON_TEXT_COLOR 0x00ffffff
#define BUTTON_SPACING 90 // px
#define BUTTON_Y_OFFSET -100 // px
#define BUTTON_WIDTH 200 // px
#define BUTTON_HEIGHT 50 // px

// groupId
#define ID_MENU 0
#define ID_CREDITS 2
#define ID_PLAY 10


/*** FUNCTION PROTOTYPES ***/
void checkButtons();
void checkBackButton();
void calculateButtons();
void initMenu();
void showTitle(int, int, GLuint texid);
void play();
void credits();
void quit();
void drawButton();
void initButton();
void showBackButton();
void back();
void calculateBackButton();

namespace andrea {
    /*** CONSTANTS ***/
    ButtonOptions buttOptions[] = { 
        { "Play", 0.5f, 0.7f, BUTTON_WIDTH, BUTTON_HEIGHT, true, BUTTON_TEXT_COLOR, ID_MENU, play },  
        { "Credits", 0.5f, 0.6f, BUTTON_WIDTH, BUTTON_HEIGHT, true, BUTTON_TEXT_COLOR, ID_MENU, credits },  
        { "Quit", 0.5f, 0.5f, BUTTON_WIDTH, BUTTON_HEIGHT, true, BUTTON_TEXT_COLOR, ID_MENU, quit},
        { "\0", 0.0f, 0.0f, 0, 0, false, 0x0, 0, 0 } 
    };
    int activeMenu;
    int initialized;
    int back_init;
     int nbuttons;
    Button buttons[4];
    Button backButton;
    ButtonOptions backButtOptions = { "Back", 0.5f, 0.6f, BUTTON_WIDTH, 
        BUTTON_HEIGHT, true, BUTTON_TEXT_COLOR, ID_MENU, back };
    int buttonGroupIds[4];
}

void initMenu()
{
    Rect rec;
    Button newButton;

    glEnable(GL_COLOR_MATERIAL);

    int i = 0;
    //main menu background image
    showTitle(1400/2, 1000/2, gl.animalEleTexture);

    while (strcmp(andrea::buttOptions[i].text, "\0")) {
        // dimensions of button
        ButtonOptions *options = &andrea::buttOptions[i];

        rec.width = options->width;
        rec.height = options->height;
        rec.left = (int)(gl.xres * options->x);
        rec.bot = (int)(gl.yres * options->y);
        rec.right = rec.left + rec.width;
        rec.top = rec.bot + rec.height;
        rec.centerx = (rec.left + rec.right) / 2;
        rec.centery = (rec.bot + rec.top) / 2;

        //button attributes and states
        strcpy(newButton.text, options->text);
        newButton.color[0] = 0.1f;
        newButton.color[1] = 0.1f;
        newButton.color[2] = 0.1f;
        newButton.color[3] = 1.0;

        newButton.dcolor[0] = 0.5f;
        newButton.dcolor[1] = 0.5f;
        newButton.dcolor[2] = 0.5f;
        newButton.dcolor[3] = 1.0 * 0.5f;
 
        newButton.id = i;
        newButton.r = rec;
        newButton.down = 0;
        newButton.click = 0;
        newButton.text_color = BUTTON_TEXT_COLOR;
        newButton.onClick = options->onClick;

        andrea::buttons[i++] = newButton;
        andrea::nbuttons++;
        
        Log("initMenu() -- buttId: %d | centerx: %d | centery: %d\n", i, rec.centerx, rec.centery);
    }
}

//adjust button location according to screen size
void calculateButtons() {
    int x = gl.xres / 2; // x position at middle
    int y = (gl.yres - (gl.yres / 3)) + BUTTON_Y_OFFSET; // y position at first third portion

    for (int i = 0; i < andrea::nbuttons; ++i) {
        Button *button = &andrea::buttons[i];
       
        button->r.width = andrea::buttOptions[i].width;
        button->r.height = andrea::buttOptions[i].height;
        button->r.left = x;
        button->r.bot = y;
        button->r.right = button->r.left + button->r.width;
        button->r.top = button->r.bot + button->r.height;
        button->r.centerx = (button->r.left + button->r.right) / 2;
        button->r.centery = (button->r.bot + button->r.top) / 2;
        y -= BUTTON_SPACING;
        Log("calculateButtons() -- %s -> r.left: %d -- r.right: %d -- r.bot: %d -- r.top: %d\n",
                button->text, button->r.left, button->r.right, button->r.bot, button->r.top);
    }
}

void calculateBackButton()
{
     int x = gl.xres / 2; // x position at middle
    int y = (gl.yres - (gl.yres / 3)) + BUTTON_Y_OFFSET; // y position at first third portion

        Button *button = &andrea::backButton;

        button->r.width = andrea::backButtOptions.width;
        button->r.height = andrea::backButtOptions.height;
        button->r.left = x;
        button->r.bot = y;
        button->r.right = button->r.left + button->r.width;
        button->r.top = button->r.bot + button->r.height;
        button->r.centerx = (button->r.left + button->r.right) / 2;
        button->r.centery = (button->r.bot + button->r.top) / 2;
        y -= BUTTON_SPACING;
        Log("calculateButtons() -- %s -> r.left: %d -- r.right: %d -- r.bot: %d -- r.top: %d\n",
                button->text, button->r.left, button->r.right, button->r.bot, button->r.top);
    

}

void showMenu()
{
    Rect r;

    if (!andrea::initialized) {
        Log("initMenu(): Initializing main menu\n");
        initMenu();
        andrea::initialized = 1;
    }
     checkButtons(); // check state of the buttons on screen
   
    showTitle(1400/2, 1000/2, gl.animalEleTexture);
    glBegin(GL_QUADS);

    glTexCoord2f(0.0f, 0.0f); glVertex2i(0, 0);
    glTexCoord2f(1.0f, 0.0f); glVertex2i(0, gl.yres);
    glTexCoord2f(1.0f, 1.0f); glVertex2i(gl.xres, gl.yres);
    glTexCoord2f(0.0f, 1.0f); glVertex2i(gl.xres, 0);
    glEnd(); 

    for (int i = 0; i < andrea::nbuttons; i++) {
        Button *button = &andrea::buttons[i];

        if (button->over) {
            //draw a highlight around button
            glColor3f(1.0f, 0.0f, 5.0f);
            glLineWidth(2);
            glBegin(GL_LINE_LOOP);
            glVertex2i(button->r.left-2,  button->r.bot-2);
            glVertex2i(button->r.left-2,  button->r.top+2);
            glVertex2i(button->r.right+2, button->r.top+2);
            glVertex2i(button->r.right+2, button->r.bot-2);
            glVertex2i(button->r.left-2,  button->r.bot-2);
            glEnd();
            glLineWidth(1);
        }
        // set color for button
        if (button->down) {
            glColor3fv(button->dcolor);

        } else {
            glColor3fv(button->color);
        }
        // draw a button
        glBegin(GL_QUADS);
        glVertex2i(button->r.left,  button->r.bot);
        glVertex2i(button->r.left,  button->r.top);
        glVertex2i(button->r.right, button->r.top);
        glVertex2i(button->r.right, button->r.bot);
        glEnd();
       // text position
        r.left = button->r.centerx;
        r.bot  = button->r.centery-8;
        r.center = 1;
        // check button state
        // click state means the button is
        if (button->down) {
            ggprint16(&r, 0, button->text_color, "Pressed!");
            Log("%s button clicked\n", button->text);
            try {
                button->onClick();
            } catch (const std::exception& e) {
                Log("Error: Button::onClick id: %d - \
                        Exception caught in event handler:\n\t%s",button->id, e.what());
                throw e;
            }
        } else {
            ggprint16(&r, 0, button->text_color, button->text);
        }
    }
}

void showBackButton()
{
//  Rect r;

    if (!andrea::back_init) {
        Log("initMenu(): Initializing main menu\n");
        initButton();
        andrea::back_init = 1;
    }   
     checkBackButton(); // check state of the buttons on screen
    //draw buttons, highlight the button with mouse cursor over


    glTexCoord2f(0.0f, 0.0f); glVertex2i(0, 0); 
    glTexCoord2f(1.0f, 0.0f); glVertex2i(0, gl.yres);
    glTexCoord2f(1.0f, 1.0f); glVertex2i(gl.xres, gl.yres);
    glTexCoord2f(0.0f, 1.0f); glVertex2i(gl.xres, 0); 
    glEnd();

    drawButton();

}
// initiating button
void initButton()
{
     Rect rec;
    Button newButton;

    int i = 0;
     ButtonOptions *options = &andrea::backButtOptions;

        rec.width = options->width;
        rec.height = options->height;
        rec.left = (int)(gl.xres * options->x);
        rec.bot = (int)(gl.yres * options->y);
        rec.right = rec.left + rec.width;
        rec.top = rec.bot + rec.height;
        rec.centerx = (rec.left + rec.right) / 2;
        rec.centery = (rec.bot + rec.top) / 2;

        //button attributes and states
        strcpy(newButton.text, options->text);
        newButton.color[0] = 0.1f;
        newButton.color[1] = 0.1f;
        newButton.color[2] = 0.1f;
        newButton.color[3] = 1.0;

        newButton.dcolor[0] = 0.5f;
        newButton.dcolor[1] = 0.5f;
        newButton.dcolor[2] = 0.5f;
        newButton.dcolor[3] = 1.0 * 0.5f;
        
        newButton.id = i;
        newButton.r = rec;
        newButton.down = 0;
        newButton.click = 0;
        newButton.text_color = BUTTON_TEXT_COLOR;
        newButton.onClick = options->onClick;

        andrea::backButton = newButton;
        Log("initMenu() -- buttId: %d | centerx: %d | centery: %d\n", i, rec.centerx, rec.centery);

}
// drawing back button
void drawButton()
{
    Rect r;
   Button *button = &andrea::backButton;

        if (button->over) {
            //draw a red highlight around button
            glColor3f(1.0f, 0.0f, 5.0f);
            glLineWidth(2);
            glBegin(GL_LINE_LOOP);
            glVertex2i(button->r.left-2,  button->r.bot-2);
            glVertex2i(button->r.left-2,  button->r.top+2);
            glVertex2i(button->r.right+2, button->r.top+2);
            glVertex2i(button->r.right+2, button->r.bot-2);
            glVertex2i(button->r.left-2,  button->r.bot-2);
            glEnd();
            glLineWidth(1);
        }
        // set color for button
        if (button->down) {
            glColor3fv(button->dcolor);

        } else {
            glColor3fv(button->color);
        }
        // draw a button
        glBegin(GL_QUADS);
        glVertex2i(button->r.left,  button->r.bot);
        glVertex2i(button->r.left,  button->r.top);
        glVertex2i(button->r.right, button->r.top);
        glVertex2i(button->r.right, button->r.bot);
        glEnd();
       // text position
        r.left = button->r.centerx;
        r.bot  = button->r.centery-8;
        r.center = 1;
        // check button state
        // click state means the button is
        if (button->down) {
            ggprint16(&r, 0, button->text_color, "Pressed!");
            Log("%s button clicked\n", button->text);
            try {
                button->onClick();
            } catch (const std::exception& e) {
                Log("Error: Button::onClick id: %d - \
                        Exception caught in event handler:\n\t%s",button->id, e.what());
                throw e;
            }
        } else {
            ggprint16(&r, 0, button->text_color, button->text);
        }

}
void DrawMouse(int mouseX, int mouseY)
{
    glColor3f(0.0, 1.0, 0.0);
        glPushMatrix();
        glTranslatef(mouseX, mouseY, 0.0);
        glBegin(GL_TRIANGLES);
            glVertex2f(-12.0f, -10.0f);
            glVertex2f(  0.0f, 20.0f);
            glVertex2f(  0.0f, -6.0f);
            glVertex2f(  0.0f, -6.0f);
            glVertex2f(  0.0f, 20.0f);
            glVertex2f( 12.0f, -10.0f);
        glEnd();
        glColor3f(1.0f, 0.0f, 0.0f);
        glBegin(GL_POINTS);
        glVertex2f(0.0f, 0.0f);
        glEnd();
        glPopMatrix();
}
//play button will go here
void play()
{
    Log("Play button clicked\n");
    gl.mainMenu = 0;
    andrea::activeMenu = ID_PLAY;
}

//credits button will go here
void credits()
{
    Log("Credits button clicked\n");
    gl.credits = 1;
    andrea::activeMenu = ID_CREDITS;
    gl.mainMenu = 0;
    andrea::activeMenu = ID_MENU;
    checkBackButton();

}

//quit button will go here
void quit()
{
    Log("Quit button clicked\n");
    exit(0);
}
// knowing you've clicked the back button
void back()
{

    Log("Back button clicked\n");
    gl.credits = 0;
    gl.mainMenu = 1;
    andrea::activeMenu = ID_MENU;
}
// checks if mouse cursor is over a button
void checkButtons()
{
    for (int i = 0; i < andrea::nbuttons; i++) {
        Button *button = &andrea::buttons[i];

        button->over = 0;
        button->down = 0;

        int compliment_y = abs(gl.yres - gl.mouse.y);
        // check for cursor over button
        Log("checkButtons() -- &gl.mouse: %x\n", &gl.mouse);
        Log("mouse x: %d -- r.left: %d -- r.right: %d\nmouse y: %d -- r.bot: %d -- r.top: %d\n",
               gl.mouse.x, button->r.left, button->r.right, gl.mouse.y,
              button->r.bot, button->r.top);
        
        if (gl.mouse.x >= button->r.left &&
               gl.mouse.x <= button->r.right &&
                compliment_y >= button->r.bot &&
                compliment_y <= button->r.top) {
            button->over = 1;
            button->down = gl.mouse.lbutton || gl.mouse.rbutton ? 1 : 0;
            Log("%s -> down: %d, over: %d\n",
                    button->text, button->down, button->over);
        }
    }
}
// checks if back button was clicked
void checkBackButton()
{
    Button *button = &andrea::backButton;

        button->over = 0;
        button->down = 0;

        int compliment_y = abs(gl.yres - gl.mouse.y);
        // check for cursor over button
        Log("checkButtons() -- &gl.mouse: %x\n", &gl.mouse);
        Log("mouse x: %d -- r.left: %d -- r.right: %d\nmouse y: %d -- r.bot: %d -- r.top: %d\n",
               gl.mouse.x, button->r.left, button->r.right, gl.mouse.y,
              button->r.bot, button->r.top);

        if (gl.mouse.x >= button->r.left &&
               gl.mouse.x <= button->r.right &&
                compliment_y >= button->r.bot &&
                compliment_y <= button->r.top) {
            button->over = 1;
            button->down = gl.mouse.lbutton || gl.mouse.rbutton ? 1 : 0;
            Log("%s -> down: %d, over: %d\n",
                    button->text, button->down, button->over);
        }

}
void show_andreas_credits(int x, int y) {
    Rect r;
    r.bot = y;
    r.left = x;
    r.center = 1;
    ggprint16(&r, 16, 0x00ffff44, "Andrea did something");
}
void showTitle(int x, int y, GLuint texid) {
   /* Rect r;
    r.bot = y;
    r.left = x;
    r.center = 1;
    ggprint40(&r, 16, 0x00ffff44, "Animal Elements");
    */
    glColor3ub(255, 255, 255);
	int width3 = 800;
	glPushMatrix();
	glTranslated(x, y, 0);
	glBindTexture(GL_TEXTURE_2D, texid);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f); glVertex2i(-width3, -width3);
	glTexCoord2f(0.0f, 0.0f); glVertex2i(-width3, width3);
	glTexCoord2f(1.0f, 0.0f); glVertex2i(width3, width3);
	glTexCoord2f(1.0f, 1.0f); glVertex2i(width3, -width3);
	glEnd();
	glPopMatrix();
}
