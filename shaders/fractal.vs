#version 410 core

uniform vec2 viewPos;
uniform float viewZoom;
uniform float viewRatio;

layout(location = 0) in vec2 VertexPosition;

out vec2 pos;

void main(void) {
//	pos = VertexPosition;
	pos = VertexPosition/viewZoom*vec2(viewRatio, 1.0f)+viewPos;
	gl_Position = vec4(VertexPosition.x, VertexPosition.y, 0.0f, 1.0f);
}
