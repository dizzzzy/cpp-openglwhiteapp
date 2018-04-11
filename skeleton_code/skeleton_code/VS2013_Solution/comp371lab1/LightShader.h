#ifndef LIGHT_SHADER_H
#define LIGHT_SHADER_H
#include <string>
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include <glew.h>

class LightShader{
private:
	//
public:
	GLuint shdr; //ShaderID
	LightShader(std::string vShader, std::string fShader);
	void use();
	void setMm(const glm::mat4&);
	void setVm(const glm::mat4&);
	void setPm(const glm::mat4&);
	

};
#endif