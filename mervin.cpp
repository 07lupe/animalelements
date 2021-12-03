/*
//3350
//Author:   Mario Ervin
//Date:     9/30/2021

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "log.h"
//#include "ppm.h"
#include "fonts.h"
#include <string>

//defined types
typedef double Flt;
typedef double Vec[3];
typedef Flt	Matrix[4][4];

//macros
#define rnd() (((double)rand())/(double)RAND_MAX)
#define random(a) (rand()%a)
#define MakeVector(v, x, y, z) (v)[0]=(x),(v)[1]=(y),(v)[2]=(z)
#define VecCopy(a,b) (b)[0]=(a)[0];(b)[1]=(a)[1];(b)[2]=(a)[2]
#define VecDot(a,b)	((a)[0]*(b)[0]+(a)[1]*(b)[1]+(a)[2]*(b)[2])
#define VecSub(a,b,c) (c)[0]=(a)[0]-(b)[0]; \
                      (c)[1]=(a)[1]-(b)[1]; \
                      (c)[2]=(a)[2]-(b)[2]
//constants
const float timeslice = 1.0f;
const float gravity = -0.2f;
#define ALPHA 1

//function prototypes
void mervin_initOpenmeg();
void mervin_checkMouse(XEvent *m);
int mervin_checkKeys(XEvent *m);
void mervin_init();
void mervin_physics();
void mervin_render();

//-----------------------------------------------------------------------------
//Setup timers
class Timer {
public:
	double physicsRate;
	double oobillion;
	struct timespec timeStart, timeEnd, timeCurrent;
	struct timespec walkTime;
	Timer() {
		physicsRate = 1.0 / 30.0;
		oobillion = 1.0 / 1e9;
	}
	double timeDiff(struct timespec *start, struct timespec *end) {
		return (double)(end->tv_sec - start->tv_sec ) +
				(double)(end->tv_nsec - start->tv_nsec) * oobillion;
	}
	void timeCopy(struct timespec *dest, struct timespec *source) {
		memcpy(dest, source, sizeof(struct timespec));
	}
	void recordTime(struct timespec *t) {
		clock_gettime(CLOCK_REALTIME, t);
	}
} timer;

class Image;

class Global {
public:
	unsigned char keys[65536];
	int xres, yres;
	//int movie, movieStep;
	int walk;
	int walkFrame;
	double delay;
    //
    int instructions;
    int show_credits;
	int score;
	Image *walkImage;
	GLuint walkTexture;
	Vec box[20];
	//Sprite exp;
	//Sprite exp44;
//	Vec ball_pos;
//	Vec ball_vel;
	//camera is centered at (0,0) lower-left of screen. 
	Flt camera[2];
	~Global() {
		logClose();
	}
	Global() {

	    score =0;
		logOpen();
		camera[0] = camera[1] = 0.0;
		//movie=0;
		//movieStep=2;
		xres=900;
		yres=700;
		walk=0;
		walkFrame=0;
		walkImage=NULL;
		//MakeVector(ball_pos, 520.0, 0, 0);
		//MakeVector(ball_vel, 0, 0, 0);
		delay = 0.1;
        //
        //instructions = 0;
        show_credits = 0;
		//exp.onoff=0;
		//exp.frame=0;
		//exp.image=NULL;
		//exp.delay = 0.02;
		//exp44.onoff=0;
		//exp44.frame=0;
		//exp44.image=NULL;
    }
} meg;

class Level {
public:
	unsigned char arr[16][80];
	int nrows, ncols;
	int tilesize[2];
	Flt ftsz[2];
	Flt tile_base;
	Level() {
		//Log("Level constructor\n");
		tilesize[0] = 32;
		tilesize[1] = 32;
		ftsz[0] = (Flt)tilesize[0];
		ftsz[1] = (Flt)tilesize[1];
		tile_base = 220.0;
		//read level
		FILE *fpi = fopen("level1.txt","r");
		if (fpi) {
			nrows=0;
			char line[100];
			while (fgets(line, 100, fpi) != NULL) {
				removeCrLf(line);
				int slen = strlen(line);
				ncols = slen;
				//Log("line: %s\n", line);
				for (int j=0; j<slen; j++) {
					arr[nrows][j] = line[j];
				}
				++nrows;
			}
			fclose(fpi);
			//printf("nrows of background data: %i\n", nrows);
		}
		for (int i=0; i<nrows; i++) {
			for (int j=0; j<ncols; j++) {
				printf("%c", arr[i][j]);
			}
			printf("\n");
		}
	}
	void removeCrLf(char *str) {
		//remove carriage return and linefeed from a Cstring
		char *p = str;
		while (*p) {
			if (*p == 10 || *p == 13) {
				*p = '\0';
				break;
			}
			++p;
		}
    }
} lev_2;

//X Windows variables
class X11_wrapper {
private:
	Display *dpy;
	Window win;
public:
	~X11_wrapper() {
		XDestroyWindow(dpy, win);
		XCloseDisplay(dpy);
	}
	void setTitle() {
		//Set the window title bar.
		XMapWindow(dpy, win);
		XStoreName(dpy, win, "3350 - Walk Cycle");
	}
	void setupScreenRes(const int w, const int h) {
		meg.xres = w;
		meg.yres = h;
	}
	X11_wrapper() {
		GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
		//GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, None };
		XSetWindowAttributes swa;
		setupScreenRes(meg.xres, meg.yres);
		dpy = XOpenDisplay(NULL);
		if (dpy == NULL) {
			printf("\n\tcannot connect to X server\n\n");
			exit(EXIT_FAILURE);
		}
		Window root = DefaultRootWindow(dpy);
		XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
		if (vi == NULL) {
			printf("\n\tno appropriate visual found\n\n");
			exit(EXIT_FAILURE);
		} 
		Colormap cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
		swa.colormap = cmap;
		swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
			StructureNotifyMask | SubstructureNotifyMask;
		win = XCreateWindow(dpy, root, 0, 0, meg.xres, meg.yres, 0,
			vi->depth, InputOutput, vi->visual,
			CWColormap | CWEventMask, &swa);
		GLXContext glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
		glXMakeCurrent(dpy, win, glc);
		setTitle();
	}
	void reshapeWindow(int width, int height) {
		//window has been resized.
		setupScreenRes(width, height);
		glViewport(0, 0, (GLint)width, (GLint)height);
		glMatrixMode(GL_PROJECTION); glLoadIdentity();
		glMatrixMode(GL_MODELVIEW); glLoadIdentity();
		glOrtho(0, meg.xres, 0, meg.yres, -1, 1);
		setTitle();
	}
	void checkResize(XEvent *m) {
		//The ConfigureNotify is sent by the
		//server if the window is resized.
		if (m->type != ConfigureNotify)
			return;
		XConfigureEvent xce = m->xconfigure;
		if (xce.width != meg.xres || xce.height != meg.yres) {
			//Window size did change.
			reshapeWindow(xce.width, xce.height);
		}
	}
	bool getXPending() {
		return XPending(dpy);
	}
	XEvent getXNextEvent() {
		XEvent m;
		XNextEvent(dpy, &m);
		return m;
	}
	void swapBuffers() {
		glXSwapBuffers(dpy, win);
	}
} x11_2;

class Image {
public:
	int width, height;
	unsigned char *data;
	~Image() { delete [] data; }
	Image(const char *fname) {
		if (fname[0] == '\0')
			return;
		//printf("fname **%s**\n", fname);
		int ppmFlag = 0;
		char name[40];
		strcpy(name, fname);
		int slen = strlen(name);
		char ppmname[80];
		if (strncmp(name+(slen-4), ".ppm", 4) == 0)
			ppmFlag = 1;
		if (ppmFlag) {
			strcpy(ppmname, name);
		} else {
			name[slen-4] = '\0';
			//printf("name **%s**\n", name);
			sprintf(ppmname,"%s.ppm", name);
			//printf("ppmname **%s**\n", ppmname);
			char ts[100];
			//system("convert eball.jpg eball.ppm");
			sprintf(ts, "convert %s %s", fname, ppmname);
			system(ts);
		}
		//sprintf(ts, "%s", name);
		//printf("read ppm **%s**\n", ppmname); fflush(stdout);
		FILE *fpi = fopen(ppmname, "r");
		if (fpi) {
			char line[200];
			fgets(line, 200, fpi);
			fgets(line, 200, fpi);
			//skip comments and blank lines
			while (line[0] == '#' || strlen(line) < 2)
				fgets(line, 200, fpi);
			sscanf(line, "%i %i", &width, &height);
			fgets(line, 200, fpi);
			//get pixel data
			int n = width * height * 3;			
			data = new unsigned char[n];			
			for (int i=0; i<n; i++)
				data[i] = fgetc(fpi);
			fclose(fpi);
		} else {
			printf("ERROR opening image: %s\n",ppmname);
			exit(0);
		}
    }
};

Image char_img[1] = {"./images/char_sprite.gif"};


int mervin_main(void)
{
    mervin_initOpenmeg();
	mervin_init();
	int done = 0;
	while (!done) {
		while (x11_2.getXPending()) {
			XEvent m = x11_2.getXNextEvent();
			x11_2.checkResize(&m);
			mervin_checkMouse(&m);
			done = mervin_checkKeys(&m);
			meg.score++;
		}
		mervin_physics();
		mervin_render();
		x11_2.swapBuffers();
	}
	cleanup_fonts();
	return 0;
}

unsigned char *mervin_buildAlphaData(Image *char_img)
{
	//add 4th component to RGB stream...
	int i;
	unsigned char *newdata, *ptr;
	unsigned char *data = (unsigned char *)char_img->data;
	newdata = (unsigned char *)malloc(char_img->width * char_img->height * 2);
	ptr = newdata;
	unsigned char a,b,c;
	//use the first pixel in the image as the transparent color.
	unsigned char t0 = *(data+0);
	unsigned char t1 = *(data+1);
	unsigned char t2 = *(data+2);
	for (i=0; i<char_img->width * char_img->height * 1.5; i+=3) {
		a = *(data+0);
		b = *(data+1);
		c = *(data+2);
		*(ptr+0) = a;
		*(ptr+1) = b;
		*(ptr+2) = c;
		*(ptr+3) = 1;
		if (a==t0 && b==t1 && c==t2)
			*(ptr+3) = 0;
		//-----------------------------------------------
		ptr += 4;
		data += 3;
	}
	return newdata;
}


void mervin_initOpenmeg(void)
{
	//OpenGL initialization
	//glViewport(0, 0, meg.xres, meg.yres);
	//Initialize matrices
	//glMatrixMode(GL_PROJECTION); glLoadIdentity();
	//lMatrixMode(GL_MODELVIEW); glLoadIdentity();
	//This sets 2D mode (no perspective)
	glOrtho(0, meg.xres, 0, meg.yres, -1, 1);
	//
	//glDisable(GL_LIGHTING);
	//glDisable(GL_DEPTH_TEST);
	//glDisable(GL_FOG);
    //glDisable(GL_CULL_FACE);
	//
	//Clear the screen
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	//Do this to allow fonts
    // 
	glEnable(GL_TEXTURE_2D);
	initialize_fonts();
	
	//load the images file into a ppm structure.
	//
	int w = char_img[0].width * 2.00;
	int h = char_img[0].height / 1.50;

	//
	//create opengl texture elements
	glGenTextures(1, &meg.walkTexture);
	//-------------------------------------------------------------------------
	//silhouette
	//this is similar to a sprite graphic
	//
	glBindTexture(GL_TEXTURE_2D, meg.walkTexture);
	//
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	//
	//must build a new set of data...
	unsigned char *walkData = mervin_buildAlphaData(&char_img[0]);	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, walkData);
	free(walkData);
}

void mervin_init() {

}

void mervin_checkMouse(XEvent *m)
{
	//printf("checkMouse()...\n"); fflush(stdout);
	//Did the mouse move?
	//Was a mouse button clicked?
	static int savex = 0;
	static int savey = 0;
	//
	if (m->type != ButtonRelease && m->type != ButtonPress &&
			m->type != MotionNotify)
		return;
	if (m->type == ButtonRelease) {
		return;
	}
	if (m->type == ButtonPress) {
		if (m->xbutton.button==1) {
			//Left button is down
		}
		if (m->xbutton.button==3) {
			//Right button is down
		}
	}
	if (m->type == MotionNotify) {
		if (savex != m->xbutton.x || savey != m->xbutton.y) {
			//Mouse moved
			savex = m->xbutton.x;
			savey = m->xbutton.y;
		}
	}
}

void mervin_screenCapture()
{
	static int fnum = 0;
	static int vid = 0;
	if (!vid) {
		system("mkdir ./vid");
		vid = 1;
	}
	unsigned char *data = (unsigned char *)malloc(meg.xres * meg.yres * 3);
    glReadPixels(0, 0, meg.xres, meg.yres, GL_RGB, GL_UNSIGNED_BYTE, data);
	char ts[32];
	sprintf(ts, "./vid/pic%03i.ppm", fnum);
	FILE *fpo = fopen(ts,"w");	
	if (fpo) {
		fprintf(fpo, "P6\n%i %i\n255\n", meg.xres, meg.yres);
		unsigned char *p = data;
		//go backwards a row at a time...
		p = p + ((meg.yres-1) * meg.xres * 3);
		unsigned char *start = p;
		for (int i=0; i<meg.yres; i++) {
			for (int j=0; j<meg.xres*3; j++) {
				fprintf(fpo, "%c",*p);
				++p;
			}
			start = start - (meg.xres*3);
			p = start;
		}
		fclose(fpo);
		char s[256];
		sprintf(s, "convert ./vid/pic%03i.ppm ./vid/pic%03i.gif", fnum, fnum);
		system(s);
		unlink(ts);
	}
	++fnum;
}

int mervin_checkKeys(XEvent *m)
{
	//keyboard input?
	static int shift=0;
	if (m->type != KeyPress && m->type != KeyRelease)
		return 0;
	int key = XLookupKeysym(&m->xkey, 0);
	meg.keys[key]=1;
	if (m->type == KeyRelease) {
		meg.keys[key]=0;
		if (key == XK_Shift_L || key == XK_Shift_R)
			shift=0;
		return 0;
	}
	meg.keys[key]=1;
	if (key == XK_Shift_L || key == XK_Shift_R) {
		shift=1;
		return 0;
	}
	(void)shift;
	switch (key) {
        case XK_w:
            timer.recordTime(&timer.walkTime);
            meg.walk ^= 2;
            break;
		case XK_Left:
			break;
		case XK_Right:
			break;
		case XK_equal:
			meg.delay -= 0.005;
			if (meg.delay < 0.005)
				meg.delay = 0.005;
			break;
        case XK_i:
            meg.instructions ^= 1;
            break;
		case XK_minus:
			meg.delay += 0.005;
			break;
		case XK_Escape:
			return 1;
			break;
	}
	return 0;
}

void mervin_physics(void)
{
	if (meg.walk || meg.keys[XK_Right] || meg.keys[XK_Left]) {
		//man is walking...
		//when time is up, advance the frame.
		timer.recordTime(&timer.timeCurrent);
		double timeSpan = timer.timeDiff(&timer.walkTime, &timer.timeCurrent);
		if (timeSpan > meg.delay) {
			//advance
			++meg.walkFrame;
			if (meg.walkFrame >= 4)
				meg.walkFrame -= 4;
			timer.recordTime(&timer.walkTime);
		}
		for (int i=0; i<20; i++) {
			if (meg.keys[XK_Left]) {
				meg.box[i][0] += 1.0 * (0.05 / meg.delay);
                //change from 15 to 10 on loop below. 
				if (meg.box[i][0] > meg.xres + 20.0)
					meg.box[i][0] -= meg.xres + 20.0;
				meg.camera[0] -= 2.0/lev_2.tilesize[0] * (0.05 / meg.delay);
				if (meg.camera[0] < 0.0)
					meg.camera[0] = 0.0;
			} else {
				meg.box[i][0] -= 1.0 * (0.05 / meg.delay);
				if (meg.box[i][0] < -10.0)
					meg.box[i][0] += meg.xres + 10.0;
				meg.camera[0] += 2.0/lev_2.tilesize[0] * (0.05 / meg.delay);
				if (meg.camera[0] < 0.0)
					meg.camera[0] = 0.0;
			}
		}
    }
}
	
void mervin_render(void)
{
	Rect r;
	glClearColor(0.1, 0.1, 0.1, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	float cx = meg.xres/2.0;
	float cy = meg.yres/2.0;

	//show ground
	glBegin(GL_QUADS);
		glColor3f(0.2, 0.2, 0.2);
		glVertex2i(0,220);
		glVertex2i(meg.xres, 220);
		glColor3f(0.4, 0.4, 0.4);
		glVertex2i(meg.xres,   0);
		glVertex2i(0,         0);
	glEnd();
	//
	

	//show boxes as background
	for (int i=0; i<20; i++) {
		glPushMatrix();
		glTranslated(meg.box[i][0],meg.box[i][1],meg.box[i][2]);
		glColor3f(0.2, 0.2, 0.2);
		glBegin(GL_QUADS);
			glVertex2i( 0,  0);
			glVertex2i( 0, 30);
			glVertex2i(20, 30);
			glVertex2i(20,  0);
		glEnd();
		glPopMatrix();
	}

	//========================
	//Render the tile system
	//========================
	int tx = lev_2.tilesize[0];
	int ty = lev_2.tilesize[1];
	Flt dd = lev_2.ftsz[0];
	Flt offy = lev_2.tile_base;
	int ncols_to_render = meg.xres / lev_2.tilesize[0] + 2;
	int col = (int)(meg.camera[0] / dd);
	col = col % lev_2.ncols;
	//Partial tile offset must be determined here.
	//The leftmost tile might be partially off-screen.
	//cdd: camera position in terms of tiles.
	Flt cdd = meg.camera[0] / dd;
	//flo: just the integer portion
	Flt flo = floor(cdd);
	//dec: just the decimal portion
	Flt dec = (cdd - flo);
	//offx: the offset to the left of the screen to start drawing tiles
	Flt offx = -dec * dd;
	//Log("gl.camera[0]: %lf   offx: %lf\n",gl.camera[0],offx);
	for (int j=0; j<ncols_to_render; j++) {
		int row = lev_2.nrows-1;
		for (int i=0; i<lev_2.nrows; i++) {
			if (lev_2.arr[row][col] == 'w') {
				glColor3f(0.8, 0.8, 0.6);
				glPushMatrix();
				//put tile in its place
				glTranslated((Flt)j*dd+offx, (Flt)i*lev_2.ftsz[1]+offy, 0);
				glBegin(GL_QUADS);
					glVertex2i( 0,  0);
					glVertex2i( 0, ty);
					glVertex2i(tx, ty);
					glVertex2i(tx,  0);
				glEnd();
				glPopMatrix();
			}
			if (lev_2.arr[row][col] == 'b') {
				glColor3f(0.9, 0.2, 0.2);
				glPushMatrix();
				glTranslated((Flt)j*dd+offx, (Flt)i*lev_2.ftsz[1]+offy, 0);
				glBegin(GL_QUADS);
					glVertex2i( 0,  0);
					glVertex2i( 0, ty);
					glVertex2i(tx, ty);
					glVertex2i(tx,  0);
				glEnd();
				glPopMatrix();
			}
			--row;
		}
		col = (col+1) % lev_2.ncols;
	}
	glColor3f(1.0, 1.0, 0.1);
	float h = 200.0;
	float w = h * 0.5;
	glPushMatrix();
	glColor3f(1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, meg.walkTexture);
	//
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.0f);
	glColor4ub(255,255,255,255);
	int ix = meg.walkFrame % 8;
	int iy = 0;
	if (meg.walkFrame >= 8)
		iy = 1;
	float fx = (float)ix / 8.0;
	float fy = (float)iy / 2.0;
	glBegin(GL_QUADS);
		if (meg.keys[XK_Left]) {
			glTexCoord2f(fx+.125, fy+.5); glVertex2i(cx-w, cy-h);
			glTexCoord2f(fx+.125, fy);    glVertex2i(cx-w, cy+h);
			glTexCoord2f(fx,      fy);    glVertex2i(cx+w, cy+h);
			glTexCoord2f(fx,      fy+.5); glVertex2i(cx+w, cy-h);
		} else {
			glTexCoord2f(fx,      fy+.5); glVertex2i(cx-w, cy-h);
			glTexCoord2f(fx,      fy);    glVertex2i(cx-w, cy+h);
			glTexCoord2f(fx+.125, fy);    glVertex2i(cx+w, cy+h);
			glTexCoord2f(fx+.125, fy+.5); glVertex2i(cx+w, cy-h);
		}
	glEnd();
	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_ALPHA_TEST);
}
void show_mervin_credits(int x, int y)
{
    Rect r;
    r.bot = y;
    r.left = x;
    r.center = 1;
    ggprint8b(&r, 16, 0x00fff000, "Mario Ervin");
}

void instructions_htp(int yres)
{   Rect r;
    r.bot - yres - 30;
    r.left = 20;
    r.center = 0;
    ggprint8b(&r, 16, 0x4dbbbe, "HOW TO PLAY");

    //instructions
    r.bot = yres - 50;
    r.left = 20;
    r.center = 0;
    ggprint8b(&r, 16, 0xfbfbfa, "Press the right directional key(->) to move right");
}
*/
