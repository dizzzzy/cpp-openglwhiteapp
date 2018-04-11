#ifndef LAMP_H
#define LAMP_H
#include <glew.h>
#include "LightShader.h"

class Lamp{
private:
	GLuint VAO;
public:
	void init();
	void draw(LightShader*, const glm::vec3& );
};

#endif