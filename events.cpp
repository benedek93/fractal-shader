#include "config.hpp"
#include "main.hpp"

void ShadersApp::handleEvent(XEvent &event) {
	Atom wmDelete = XInternAtom(disp, "WM_DELETE_WINDOW", True);
	KeySym ksym;

	switch(event.type) {
	case MotionNotify:
		cursorX = 2.0f*((float)event.xmotion.x/curW)-1.0f;
		cursorY = 1.0f-2.0f*((float)event.xmotion.y/curH);
		if(event.xmotion.state & Button1Mask) {
			setParam();
		}
		if(event.xmotion.state & Button2Mask) {
			posX += ratio*(cursorX - moveBeginX)/zoom;
			posY += (cursorY - moveBeginY)/zoom;
			moveBeginX = cursorX;
			moveBeginY = cursorY;
		}
		if(event.xmotion.state & Button3Mask) {
			posX -= ratio*cursorX/zoom;
			posY -= cursorY/zoom;
			XWarpPointer(disp, None, win, 0,0,0,0,
			             pixelMoveBeginX, pixelMoveBeginY);
		}
		break;
	case ButtonPress:
		cursorX = 2.0f*((float)event.xbutton.x/curW)-1.0f;
		cursorY = 1.0f-2.0f*((float)event.xbutton.y/curH);
		switch(event.xbutton.button) {
		case Button1:
			setParam();
			break;
		case Button2:
			moveBeginX = cursorX;
			moveBeginY = cursorY;
			break;
		case Button3:
			pixelMoveBeginX = curW/2;
			pixelMoveBeginY = curH/2;
			XWarpPointer(disp, None, win, 0,0,0,0,
			             pixelMoveBeginX, pixelMoveBeginY);
			break;
		case Button4:
			posX -= ratio*cursorX/10.0f/zoom;
			posY -= cursorY/10.0f/zoom;
			zoom *= 1.1f;
			if(zoom >= 100000.0f) zoom = 100000.0f;
			break;
		case Button5:
			zoom /= 1.1f;
			if(zoom <= 0.1f) zoom = 0.1f;
			posX += ratio*cursorX/10.0f/zoom;
			posY += cursorY/10.0f/zoom;
			break;
		default:
			break;
		}
		break;
	case ButtonRelease:
		break;
	case KeyPress:
		ksym = XKeycodeToKeysym(disp, event.xkey.keycode, 0);
		switch(ksym) {
		case XK_d:
			movingX = -1;
			break;
		case XK_a:
			movingX = 1;
			break;
		case XK_w:
			movingY = -1;
			break;
		case XK_s:
			movingY = 1;
			break;
		case XK_q:
			zooming = -1;
			break;
		case XK_e:
			zooming = 1;
			break;
		case XK_u:
			unzoom();
			break;
		case XK_Left:
//			iterations >>= 1;
			iterations -= 8;
			if(iterations < 2) {
				iterations = 2;
			}
			cerr << "Iterations: " << iterations << endl;
			break;
		case XK_Right:
//			iterations <<= 1;
			iterations += 8;
			if(iterations > 20000) {
				iterations = 20000;
			}
			cerr << "Iterations: " << iterations << endl;
			break;
		case XK_j:
			juliaMode = !juliaMode;
			break;
		case XK_b:
			blinkMode = !blinkMode;
			break;
		case XK_f:
			fastMode = !fastMode;
			break;
		case XK_F11:
			destroyWindow();
			fullscreen = !fullscreen;
			initWindow();
			break;
		case XK_Escape:
			quitting = True;
			break;
		default:
			break;
		}
		break;
	case KeyRelease:
		ksym = XKeycodeToKeysym(disp, event.xkey.keycode, 0);
		switch(ksym) {
		case XK_d:
		case XK_a:
			movingX = 0;
			break;
		case XK_w:
		case XK_s:
			movingY = 0;
			break;
		case XK_q:
		case XK_e:
			zooming = 0;
			break;
		default:
			break;
		}
		break;
	case ConfigureNotify:
		if(event.xconfigure.width != curW || event.xconfigure.height != curH) {
			curW = event.xconfigure.width;
			curH = event.xconfigure.height;
			reshapeView();
		}
		break;
	case ClientMessage:
		if(event.xclient.data.l[0] == wmDelete) {
			quitting = True;
		}
		break;
	default:
		break;
	}
}
