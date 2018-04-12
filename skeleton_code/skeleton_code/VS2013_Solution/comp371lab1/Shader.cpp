#include "Shader.h"
#include "stdafx.h"
#include "stb_image.h"
#include <iostream>
#include <fstream>
#include <vector>

Shader::Shader(std::string vertex_shader_path, std::string fragment_shader_path){
	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_shader_path, std::ios::in);

	if (VertexShaderStream.is_open()) {
		std::string Line = "";
		while (getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ?\n", vertex_shader_path.c_str());
		getchar();
		exit(-1);
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_shader_path, std::ios::in);

	if (FragmentShaderStream.is_open()) {
		std::string Line = "";
		while (getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ?\n", fragment_shader_path.c_str());
		getchar();
		exit(-1);
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_shader_path.c_str());
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, nullptr);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, nullptr, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_shader_path.c_str());
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, nullptr);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, nullptr, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);

	glBindAttribLocation(ProgramID, 0, "in_Position");

	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, nullptr, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDeleteShader(VertexShaderID); //free up memory
	glDeleteShader(FragmentShaderID);

	this->shdr = ProgramID;
}

void Shader::use(){
	glUseProgram(this->shdr);
}

void Shader::setMm(const glm::mat4 & mm){
	this->use();
	glUniformMatrix4fv(glGetUniformLocation(this->shdr, "m_m"), 1, false, glm::value_ptr(mm));
}

void Shader::setVm(const glm::mat4 & vm){
	this->use();
	glUniformMatrix4fv(glGetUniformLocation(this->shdr, "v_m"), 1, false, glm::value_ptr(vm));
}

void Shader::setPm(const glm::mat4 & pm){
	this->use();
	glUniformMatrix4fv(glGetUniformLocation(this->shdr, "p_m"), 1, false, glm::value_ptr(pm));
}

void Shader::setX(int x_pressed){
	this->use();
	glUniform1i(glGetUniformLocation(this->shdr, "shader_x_pressed"), x_pressed);
	this->shader_x_pressed = x_pressed;
}

void Shader::setFillLoc(int fill){
	this->use();
	glUniform1i(glGetUniformLocation(shdr, "fill"), fill);
}

void Shader::setLightColor(glm::vec3 lightColor){
	this->use();
	glUniform3f(glGetUniformLocation(shdr, "lightColor"), 1.0, 1.0, 1.0);
	
}

void Shader::setLightPos(glm::vec3 l_pos){
	this->use();
	glUniform3f(glGetUniformLocation(shdr, "lightPos"), l_pos.x, l_pos.y, l_pos.z);
}

void Shader::setViewPos(glm::vec3 c_pos){
	this->use();
	glUniform3f(glGetUniformLocation(shdr, "viewPos"), c_pos.x, c_pos.y, c_pos.z);
}

void Shader::loadTexture0(const char* texturePath){
	// load and create a texture 
	// -------------------------
	// texture 0
	// ---------
	glGenTextures(1, &this->texture0);
	glBindTexture(GL_TEXTURE_2D, this->texture0);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char *data = stbi_load(texturePath, &width, &height, &nrChannels, 0); //"" container.jpg
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
		getchar();
	}
	stbi_image_free(data);

	glUniform1i(glGetUniformLocation(this->shdr, "texture0"), 0);
}

void Shader::loadTexture1(const char* texturePath){
	// load and create a texture 
	// -------------------------
	// texture 1
	// ---------
	glGenTextures(1, &this->texture1);
	glBindTexture(GL_TEXTURE_2D, this->texture1);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char *data = stbi_load(texturePath, &width, &height, &nrChannels, 0); //"" container.jpg
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
		getchar();
	}
	stbi_image_free(data);
	glUniform1i(glGetUniformLocation(this->shdr, "texture1"), 1);
}

void Shader::activateTexture0(){
	this->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->texture0);
}

void Shader::activateTexture1(){
	this->use();
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, this->texture1);
}

void Shader::UnbindTexture(){
	this->use();
	glBindTexture(GL_TEXTURE_2D, 0);
}