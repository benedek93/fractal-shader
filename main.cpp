#include "config.hpp"
#include "main.hpp"

int main(int argc, char **argv) {

	try {
		ShadersApp myApp;
		myApp.mainLoop();
	} catch(string e) {
		cerr << "ERROR: " << e << endl;
	}

	return 0;
}

ShadersApp::ShadersApp(bool fs) {
	init(DEF_W, DEF_H, fs);
}

ShadersApp::ShadersApp(int w, int h, bool fs) {
	init(w, h, fs);
}

ShadersApp::~ShadersApp() {
	destroyWindow();
}

void ShadersApp::init(int w, int h, bool fs) {
	paramX = 0.0f;
	paramY = 0.0f;
	iterations = 32;
	unzoom();
	ratio = 1.0f;
	juliaMode = false;
	blinkMode = false;

	cursorX = 0.0f;
	cursorY = 0.0f;
	moveBeginX = 0.0f;
	moveBeginY = 0.0f;
	zooming = 0;
	movingX = 0;
	movingY = 0;

	windowedW = w;
	windowedH = h;
	time = 0.0;
	fastMode = false;
	fullscreen = fs;
	quitting = false;

	initWindow();
}

void ShadersApp::initWindow() {
	int attrList[] = { GLX_RGBA, GLX_DOUBLEBUFFER, GLX_RED_SIZE, 4,
	 GLX_GREEN_SIZE, 4, GLX_BLUE_SIZE, 4, None };
	XVisualInfo *vi;
	XSetWindowAttributes swa;
	Atom wmDelete;

	disp = XOpenDisplay(NULL);
	if(disp == NULL) {
		throw string("Couldn't open display");
	}

	screen = XDefaultScreen(disp);

	vi = glXChooseVisual(disp, screen, attrList);
	if(vi == NULL) {
		throw string("Couldn't get GLX visual");
	}

	swa.colormap = XCreateColormap(disp, XRootWindow(disp, screen), vi->visual,
	                               AllocNone);
	swa.event_mask = StructureNotifyMask|KeyPressMask|KeyReleaseMask|
	                 ButtonPressMask|ButtonReleaseMask|PointerMotionMask;
	if(fullscreen) {
		windowedW = curW;
		windowedH = curH;

		curW = XWidthOfScreen(XScreenOfDisplay(disp, screen));
		curH = XHeightOfScreen(XScreenOfDisplay(disp, screen));

		swa.override_redirect = True;
	} else {
		curW = windowedW;
		curH = windowedH;

		swa.override_redirect = False;
	}
	win = XCreateWindow(disp, XRootWindow(disp, screen), 0,0,
	                    curW, curH, 0, vi->depth, InputOutput, vi->visual,
	                    CWColormap|CWEventMask|CWOverrideRedirect, &swa);
	wmDelete = XInternAtom(disp, "WM_DELETE_WINDOW", True);
	XSetWMProtocols(disp, win, &wmDelete, 1);
	XStoreName(disp, win, "GL Shaders");
	XMapRaised(disp, win);
	if(fullscreen) {
		XGrabKeyboard(disp, win, True, GrabModeAsync, GrabModeAsync,
		 CurrentTime);
		XGrabPointer(disp, win, True, ButtonPressMask, GrabModeAsync,
		 GrabModeAsync, win, None, CurrentTime);
	}

	ctx = glXCreateContext(disp, vi, 0, true);
	glXMakeCurrent(disp, win, ctx);

	if(glewInit() != GLEW_OK) {
		throw string("Couldn't initialize GLEW");
	}

	initGL();
	reshapeView();
}

void ShadersApp::mainLoop() {
	XEvent event;
	int frames = 0;
	struct timespec tsStart, tsFramecountLast, tsFramecountEnd;
	struct timespec tsLastFrame, tsCurFrame;
	float dt;

	clock_gettime(CLOCK_MONOTONIC, &tsStart);
	tsFramecountLast = tsStart;
	tsLastFrame = tsStart;

	while(!quitting) {
		while(XPending(disp)) {
			XNextEvent(disp, &event);
			handleEvent(event);
		}

		clock_gettime(CLOCK_MONOTONIC, &tsCurFrame);
		dt = (tsCurFrame.tv_sec-tsLastFrame.tv_sec) +
		 (tsCurFrame.tv_nsec-tsLastFrame.tv_nsec)/1000000000.0;
		time = (tsCurFrame.tv_sec-tsStart.tv_sec) +
		 (tsCurFrame.tv_nsec-tsStart.tv_nsec)/1000000000.0;
		tsLastFrame = tsCurFrame;

		if(movingX != 0) {
			posX += 0.8f*dt*movingX/zoom;
		}
		if(movingY != 0) {
			posY += 0.8f*dt*movingY/zoom;
		}
		if(zooming != 0) {
			zoom *= 1.0f+zooming*dt;
			if(zoom <= 0.1f) zoom = 0.1f;
			if(zoom >= 100000.0f) zoom = 100000.0f;
		}

		if(!fastMode) {
			usleep(10000);
		}
		drawScene();

		if(++frames >= RATE_COUNTER_INTERVAL) {
			float millis;
			frames = 0;
			clock_gettime(CLOCK_MONOTONIC, &tsFramecountEnd);
			millis = (tsFramecountEnd.tv_sec-tsFramecountLast.tv_sec)*1000.0 +
			 (tsFramecountEnd.tv_nsec-tsFramecountLast.tv_nsec)/1000000.0;
			printf("1 frame in %8.2f ms: %5.0f FPS\n",
			 millis/RATE_COUNTER_INTERVAL, RATE_COUNTER_INTERVAL*1000/millis);
			tsFramecountLast = tsFramecountEnd;
		}
	}
}

void ShadersApp::destroyWindow() {
	if(disp != NULL) {
		XCloseDisplay(disp);
	}
}

void ShadersApp::unzoom() {
	posX = 0.0f;
	posY = 0.0f;
	zoom = 0.7f;
}

void ShadersApp::setParam() {
	/*
	paramX = cursorX;
	paramY = cursorY;
	*/
	paramX = cursorX*ratio/zoom - posX;
	paramY = cursorY/zoom - posY;
}
