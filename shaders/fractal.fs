#version 410 core

uniform vec2 param;
uniform int iters;
uniform vec2 viewPos;
uniform float viewZoom;
uniform float viewRatio;
uniform float anim;
uniform bool juliaMode;
uniform bool blinkMode;

layout(location = 0) out vec4 FragmentColor;

in vec2 pos;

int julia(vec2 z) {
	int i;
	vec2 mu;

	if(juliaMode) {
		mu = param;
	} else {
		mu = z;
	}

	for(i=0; i<iters; i++) {
		if(length(z) >= 1000.0f) break;
		/*
		if(abs(z.x) >= 2.0f) break;
		if(abs(z.y) >= 2.0f) break;
		*/

		z = vec2(z.x*z.x - z.y*z.y, 2.0f*z.x*z.y);
//		z = vec2(z.x*z.x*z.x - 3.0*z.x*z.y*z.y, 3.0f*z.x*z.x*z.y - z.y*z.y*z.y);
//		z = vec2(5*z.x*z.y*z.y*z.y*z.y-10*z.x*z.x*z.x*z.y*z.y+z.x*z.x*z.x*z.x*z.x, z.y*z.y*z.y*z.y*z.y-10*z.x*z.x*z.y*z.y*z.y+5*z.x*z.x*z.x*z.x*z.y);

		z += mu;
	}

	if(i == iters) return -1;

	return i;
}

void main(void) {
	float r, g, b;
	int bailout;

	bailout = julia(pos);

	/*
	g = 1.0f*julia(pos)/iters;
	if(g == 1.0f) {
		r = 1.0f;
		b = 1.0f;
	} else {
		r = 0.0f;
		b = 0.0f;
	}
	*/

	/*
	r = 1.0f*julia(pos/(viewZoom*1.01)+viewPos)/iters;
	g = 1.0f*julia(pos/viewZoom+viewPos)/iters;
	b = 1.0f*julia(pos/(viewZoom*0.99)+viewPos)/iters;
	*/

	if(bailout == -1) {
		r = g = b = 1.0f;
	} else {
		if(blinkMode && bailout%32 == int(anim*32)) {
			r = 1.0f;
		} else {
			r = 0.0f;
		}
		g = 1.0f*bailout/iters;
		b = 0.0f;
	}

	FragmentColor = vec4(r, g, b, 1.0f);
}
