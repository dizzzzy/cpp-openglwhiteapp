#ifndef GRID_H
#define GRID_H
#include <glew.h>
#include "Shader.h"

class Grid{
private:
	GLuint VAO;
public:
	void init();
	void draw(Shader*);
};

#endif