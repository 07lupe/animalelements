// Daniel Macias
//
//
#include "fonts.h"
#include <cstring>
#include <cstdio>
#include <cstddef>
#include <stdlib.h>
#include <unistd.h>
#include <GL/glx.h>


typedef double Vec[3];


class Image {
	public:
		int width, height;
		unsigned char *data;
		~Image() { delete [] data; }
		Image(const char *fname) {
			if (fname[0] == '\0')
				return;
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
				sprintf(ppmname,"%s.ppm", name);
				char ts[100];
				sprintf(ts, "convert %s %s", fname, ppmname);
				system(ts);
			}
			FILE *fpi = fopen(ppmname, "r");
			if (fpi) {
				char line[200];
				fgets(line, 200, fpi);
				fgets(line, 200, fpi);
				while (line[0] == '#' || strlen(line) < 2)
					fgets(line, 200, fpi);
				sscanf(line, "%i %i", &width, &height);
				fgets(line, 200, fpi);
				int n = width * height * 3;
				data = new unsigned char[n];
				for (int i=0; i<n; i++)
					data[i] = fgetc(fpi);
				fclose(fpi);
			} else {
				printf("ERROR opening image: %s\n",ppmname);
				exit(0);
			}
			if (!ppmFlag)
				unlink(ppmname);
		}
};

Image myimg[4] = {
	"./images/walk.gif",
	"./images/exp.png",
	"./images/exp44.png",
	"./images/seamless_back.png"};

class Sprite {
	public:
		int onoff;
		int frame;
		double delay;
		float xc[2];
		float yc[2];
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

class myGlobal {
	public:

		Sprite tex;
}g;


void show_dmacias_credits(int x,int y)
{
	Rect r;
	r.bot = y  ;
	r.left = x;
	r.center = 1;
	ggprint8b(&r, 16,0x00fff000, "Daniel did some programing");

}
void dmacias_initOpeng(void)
{
	g.tex.image =&myimg[3];
	glGenTextures(1, &g.tex.tex);
	int x = g.tex.image->width;
	int y = g.tex.image->height;
	glBindTexture(GL_TEXTURE_2D, g.tex.tex);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, x, y, 0,
			GL_RGB, GL_UNSIGNED_BYTE, g.tex.image->data);

	g.tex.xc[0] = 0.0;
	g.tex.xc[1] = 0.25;
	g.tex.yc[0] = 0.0;
	g.tex.yc[1] = 1.0;



}
void show_dmacias_image(int xres , int yres)
{



	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, g.tex.tex);
	glBegin(GL_QUADS);
	glTexCoord2f(g.tex.xc[0], g.tex.yc[1]); glVertex2i(0, 0);
	glTexCoord2f(g.tex.xc[0], g.tex.yc[0]); glVertex2i(0, yres);
	glTexCoord2f(g.tex.xc[1], g.tex.yc[0]); glVertex2i(xres, yres);
	glTexCoord2f(g.tex.xc[1], g.tex.yc[1]); glVertex2i(xres, 0);
	glEnd();


	//JUMP
//	float cy;
//	float cx;

//	cy = (yres-yres) + 200;
//	cx = (xres-xres) + 90;





}
void dmacias_physics(void)
{

	//move backimage
	g.tex.xc[0] += 0.001;
	g.tex.xc[1] += 0.001;
	


}



