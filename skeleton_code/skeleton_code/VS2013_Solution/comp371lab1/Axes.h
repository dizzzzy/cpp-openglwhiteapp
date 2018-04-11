#ifndef AXES_H
#define AXES_H
#include <glew.h>
#include "Shader.h"

class Axes{
private:
	GLuint xVAO;
	GLuint yVAO;
	GLuint zVAO;
public:
	void init();
	void draw(Shader*);
};

#endif