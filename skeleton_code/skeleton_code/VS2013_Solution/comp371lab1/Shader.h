#ifndef SHADER_H
#define SHADER_H
#include <string>
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include <glew.h>

class Shader{
private:
	
	//Vertex Shader Uniforms
	/*GLuint mm_addr;
	GLuint vm_addr;
	GLuint pm_addr;*/

	//Fragment Shader Uniforms
	
	GLuint texture0;
	GLuint texture1;
	//GLuint fillLoc;
	
	/*glm::vec3 lightColor;
	glm::vec3 lightPos;
	glm::vec3 viewPos;*/

public:
	GLuint shdr; //ShaderID
	int shader_x_pressed;
	Shader(std::string vShader, std::string fShader);
	void use();
	void setMm(const glm::mat4&);
	void setVm(const glm::mat4&);
	void setPm(const glm::mat4&);
	void setX(int);
	void setFillLoc(int);
	void setLightColor(glm::vec3 lightColor);
	void setLightPos(glm::vec3 lightPos);
	void setViewPos(glm::vec3 viewPos);
	void loadTexture0(const char* texturePath);
	void loadTexture1(const char* texturePath);
	void activateTexture0();
	void activateTexture1();
	void UnbindTexture();

};
#endif