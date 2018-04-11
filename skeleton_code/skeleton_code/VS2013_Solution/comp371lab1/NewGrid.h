#ifndef NEW_GRID_H
#define NEW_GRID_H
#include <glew.h>
#include "Shader.h"

class NewGrid{
private:
	GLuint VAO;
public:
	void init();
	void draw(Shader*);
};

#endif