#include "config.hpp"
#include "main.hpp"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

static GLuint vbo, ibo;

typedef struct {
	vec2 pos;
} vertex;

static void make_buffers(void) {
	int i, j;
	vertex *verts = new vertex[4];

	verts[0].pos = vec2(-1.0f, -1.0f);

	verts[1].pos = vec2(1.0f, -1.0f);

	verts[2].pos = vec2(1.0f, 1.0f);

	verts[3].pos = vec2(-1.0f, 1.0f);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 4*sizeof(vertex), verts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), 0);

	delete [] verts;
	unsigned int *idxs = new unsigned int[4];
	idxs[0] = 3;
	idxs[1] = 0;
	idxs[2] = 2;
	idxs[3] = 1;

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4*sizeof(unsigned int), idxs, GL_STATIC_DRAW);

	delete [] idxs;
}

void ShadersApp::initGL() {
	glClearColor(0.0, 0.0, 0.0, 0.0);

	make_buffers();

	fractalVS = loadShaderFile("shaders/fractal.vs", GL_VERTEX_SHADER);
	fractalFS = loadShaderFile("shaders/fractal.fs", GL_FRAGMENT_SHADER);
	fractalPO = glCreateProgram();
	glAttachShader(fractalPO, fractalVS);
	glAttachShader(fractalPO, fractalFS);
	glLinkProgram(fractalPO);
	checkLinkError(fractalPO);
}

void ShadersApp::reshapeView() {
	glViewport(0, 0, curW, curH);

	if(curH == 0) {
		ratio = 1.0f;
	} else {
		ratio = (float)curW/curH;
	}
}

void ShadersApp::drawScene() {
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(fractalPO);
	glUniform2f(glGetUniformLocation(fractalPO, "param"), paramX, paramY);
	glUniform1i(glGetUniformLocation(fractalPO, "iters"), iterations);
	glUniform2f(glGetUniformLocation(fractalPO, "viewPos"), -posX, -posY);
	glUniform1f(glGetUniformLocation(fractalPO, "viewZoom"), zoom);
	glUniform1f(glGetUniformLocation(fractalPO, "viewRatio"), ratio);
	glUniform1f(glGetUniformLocation(fractalPO, "anim"), fmodf(time*0.5f, 1.0f));
	glUniform1i(glGetUniformLocation(fractalPO, "juliaMode"), juliaMode);
	glUniform1i(glGetUniformLocation(fractalPO, "blinkMode"), blinkMode);
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, BUFFER_OFFSET(0));

	glXSwapBuffers(disp, win);
}

GLuint ShadersApp::loadShaderFile(const char *filename, GLenum type) {
	GLuint shaderID = glCreateShader(type);

	ifstream file(filename);
	string source;

	while(file.good()) {
		int c = file.get();
		if(c >= 0) {
			source += (char)c;
		} else break;
	}
	file.close();

	glShaderSource(shaderID, 1, (const GLchar **)&source, NULL);
	glCompileShader(shaderID);

	GLint status;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &status);
	if(status != GL_TRUE) {
		cerr << "Failed to compile shader from \"" << filename << "\"" << endl;
		GLchar log[16384];
		glGetShaderInfoLog(shaderID, 16384, NULL, log);
		cerr << "Log following:" << endl;
		cerr << log << endl;
		exit(1);
	}

	return shaderID;
}

void ShadersApp::checkLinkError(GLuint prog) {
	GLint status;
	glGetProgramiv(prog, GL_LINK_STATUS, &status);
	if(status != GL_TRUE) {
		cerr << "Failed to link shaders:" << endl;
		GLchar log[16384];
		glGetProgramInfoLog(prog, 16384, NULL, log);
		cerr << "Log following:" << endl;
		cerr << log << endl;
		exit(1);
	}
}
