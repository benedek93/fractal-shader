#pragma once

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <X11/Xlib.h>
#include <glm/glm.hpp>

using namespace std;
using namespace glm;

class ShadersApp {
	public:
	ShadersApp(bool fs=False);
	ShadersApp(int w, int h, bool fs=False);
	~ShadersApp();
	void mainLoop(void);

	private:
	void init(int w, int h, bool fs);
	void initWindow();
	void destroyWindow();

	void handleEvent(XEvent &event);
	void unzoom(void);
	void setParam(void);

	void initGL();
	void reshapeView();
	void drawScene();

	GLuint loadShaderFile(const char *filename, GLenum type);
	void checkLinkError(GLuint prog);

	float paramX;
	float paramY;
	int iterations;
	float posX;
	float posY;
	float zoom;
	float ratio;
	bool juliaMode;
	bool blinkMode;

	float cursorX;
	float cursorY;
	int pixelMoveBeginX;
	int pixelMoveBeginY;
	float moveBeginX;
	float moveBeginY;
	int zooming;
	int movingX;
	int movingY;

	int curW;
	int curH;
	int windowedW;
	int windowedH;
	double time;
	bool fastMode;
	bool fullscreen;
	bool quitting;

	Display *disp;
	int screen;
	Window win;
	GLXContext ctx;

	 /* Vertex shader: */
	GLuint fractalVS;
	 /* Fragment shader: */
	GLuint fractalFS;
	 /* Program object: */
	GLuint fractalPO;
};
