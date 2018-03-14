// opengl.cpp : Defines the entry point for the console application.
//
//#define GLM_ENABLE_EXPERIMENTAL
#include "stdafx.h"

#include <glm.hpp>
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include <glew.h>
#include <glfw3.h>
#include <vector>
#include <iostream>
#include <string>
#include <fstream>

GLFWwindow* window;

GLuint mm_addr;
GLuint vm_addr;
GLuint pm_addr;
GLuint fillLoc;

glm::vec3 c_pos = glm::vec3(0, 0, 3);
glm::vec3 c_dir = glm::normalize(glm::vec3(0, 0, -3));
glm::vec3 center = glm::vec3(0, 0, 0); //were locking what the camera is looking at.
//glm::vec3 c_pos = glm::vec3(0, 0, 3);
//glm::vec3 c_dir = glm::normalize(glm::vec3(0, 0, 0));
glm::vec3 c_up = glm::vec3(0, 1, 0);

//------ MODEL MATRIX ---------
glm::mat4 mm;
glm::mat4 scale;
glm::mat4 translate;
glm::mat4 rotate;

//------ VIEW MATRIX ---------

glm::mat4 vm = glm::lookAt(c_pos, c_pos + c_dir, c_up);
/*glm::mat4 vm = glm::lookAt(c_pos, //eye
	center,	//what your looking at, in this case its the center (0,0,0)
	c_up);	//up vector that defines the worlds up direction*/

//------ PROJECTION MATRIX -------
glm::mat4 pm = glm::perspective(45.f, 800.f / 600.f, 0.1f, 100.f);

//-------- DRAWING MODE ------
char mode = 'L';

//--------MODIFICATIONS ------
glm::vec3 translateMod = glm::vec3(0, 0, 0);
glm::vec3 scaleMod = glm::vec3(1, 1, 1);
glm::vec3 rotateMod = glm::vec3(0, 0, 0);

//---------HORSE BODY PART DIMENSIONS---------
glm::vec3 leg = glm::vec3(1.5, 0.5, 0.5);  //really half a leg
glm::vec3 body = glm::vec3(5, 2, 2);

//-------MOUSE BUTTONS STATUS----------
bool right_pressed = false;
bool left_pressed = false;
bool middle_pressed = false;

//-------CURSOR POSITION UPON CLICK-----
double cursorXPos; 
double cursorYPos;


GLuint loadShaders(std::string vertex_shader_path, std::string fragment_shader_path) {

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

	return ProgramID;
}

void update_view() {
	vm = glm::lookAt(c_pos, c_pos + c_dir, c_up);
	//vm = glm::lookAt(c_pos, center, c_up);
}

void scrollCallback(GLFWwindow* window, double xOffset, double yOffset){
	std::cout << xOffset << " : " << yOffset << std::endl;
	glm::vec3 zoom;
	//c_dir = glm::normalize(center - c_pos);
	zoom = c_dir * 0.25f;
	if (yOffset > 0){ //zoom in
		c_pos += zoom;
		update_view();
	}
	else{		//zoom out
		c_pos -= zoom;
		update_view();
	}
}

void mouse_buttonCallback(GLFWwindow* window, int button, int action, int mods){
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS){
		right_pressed = true;
		glfwGetCursorPos(window, &cursorXPos, &cursorYPos);
		std::cout << "Right button pressed " << cursorXPos << ":" << cursorYPos << std::endl;
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE){
		right_pressed = false;
		std::cout << "Right button released" << std::endl;
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
		left_pressed = true;
		glfwGetCursorPos(window, &cursorXPos, &cursorYPos);
		std::cout << "Left button pressed " << cursorXPos << ":" << cursorYPos << std::endl;
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE){
		left_pressed = false;
		std::cout << "Left button released" << std::endl;
	}
	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS){
		middle_pressed = true;
		glfwGetCursorPos(window, &cursorXPos, &cursorYPos);
		std::cout << "Left button pressed " << cursorXPos << ":" << cursorYPos << std::endl;
	}
	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE){
		middle_pressed = false;
		std::cout << "Left button released" << std::endl;
	}
	
}

static void cursorPositionCallback(GLFWwindow* window, double xPos, double yPos){
	if (left_pressed){	//zoom in and out on left button press
		std::cout << xPos << ":" << yPos << std::endl;
		glm::vec3 zoom;
		//c_dir = glm::normalize(center - c_pos);
		double yDelta = cursorYPos - yPos;
		cursorYPos = yPos;
		zoom = c_dir * 0.25f;
		if (yDelta > 0){ //zoom in
			c_pos -= zoom;
			update_view();
		}
		else{		//zoom out
			c_pos += zoom;
			update_view();
		}
	}
	else if (right_pressed){
		glm::vec2 delta;
		delta.x = cursorXPos - xPos;
		cursorXPos = xPos;
		glm::mat4 rotateMatrix(1); //identityMatrix
		rotateMatrix = glm::rotate(rotateMatrix, delta.x * 0.005f, glm::vec3(0.0, 1.0, 0.0));
		c_dir = glm::vec3(rotateMatrix * glm::vec4(c_dir, 1.0));
		update_view();
		std::cout << "c_dir x: " << c_dir.x << " y: " << c_dir.y << " z: " << c_dir.z << std::endl;

	}
	else if (middle_pressed){
		glm::vec2 delta;
		delta.y = cursorYPos - yPos;
		cursorYPos = yPos;
		glm::vec3 toRotateAround = glm::cross(c_dir, c_up);
		glm::mat4 rotateMatrix(1); //identityMatrix
		rotateMatrix = glm::rotate(rotateMatrix, delta.y * 0.005f, toRotateAround);
		c_dir = glm::vec3(rotateMatrix * glm::vec4(c_dir, 1.0));
		update_view();
		std::cout << "c_dir x: " << c_dir.x << " y: " << c_dir.y << " z: " << c_dir.z << std::endl;
	}

}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	
	printf("%d\n", key);

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
		c_pos.y += 0.5;
		update_view();
	}

	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
		c_pos.y -= 0.5;
		update_view();
	}

	if (key == GLFW_KEY_LEFT) {
		c_pos.x -= 0.5;
		update_view();
	}

	if (key == GLFW_KEY_RIGHT) {
		c_pos.x += 0.5;
		update_view();
	}

	if (key == GLFW_KEY_W) {
		if (mods == GLFW_MOD_SHIFT) {
			translateMod.z -= 1;
		}
		else {
			rotateMod.z -= 5;
		}
	}

	if (key == GLFW_KEY_A) {
		if (mods == GLFW_MOD_SHIFT) {
			translateMod.x -= 1;
		}
		else {
			rotateMod.y += 5; 
		}
	}

	if (key == GLFW_KEY_S) {
		if (mods == GLFW_MOD_SHIFT) {
			translateMod.z += 1;
		}
		else {
			rotateMod.z += 5;
		}
	}

	if (key == GLFW_KEY_D) {
		if (mods == GLFW_MOD_SHIFT) {
			translateMod.x += 1;
		}
		else {
			rotateMod.y -= 5;
		}
	}

	if (key == GLFW_KEY_T && action == GLFW_PRESS) {
		mode = 'T';
	}
	
	if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		mode = 'P';
	}

	if (key == GLFW_KEY_L && action == GLFW_PRESS) {
		mode = 'L';
	}

	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		//relocate to random place on map
		translateMod.x = float(rand() % 40 + -20);
		translateMod.z = float(rand() % 40 + -20);
	}

	if (key == GLFW_KEY_U && action == GLFW_PRESS) {
		//relocate to random place on map
		scaleMod.x *= 1.05;
		scaleMod.y *= 1.05;
		scaleMod.z *= 1.05;
	}

	if (key == GLFW_KEY_J && action == GLFW_PRESS) {
		//relocate to random place on map
		scaleMod.x /= 1.05;
		scaleMod.y /= 1.05;
		scaleMod.z /= 1.05;
	}
}

int init() {

	glfwInit();

	window = glfwCreateWindow(800, 800, "First window", nullptr, nullptr);

	glfwSetKeyCallback(window, key_callback);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetMouseButtonCallback(window, mouse_buttonCallback);
	glfwSetCursorPosCallback(window, cursorPositionCallback);

	if (window == nullptr) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (glewInit() != 0) {
		return -1;
	}

	return 0;
}

GLuint initCube(){
	GLfloat cube_vertices[] = { // 1x1x1 cube centered at (0,0,0)
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, 0.5f,
		-0.5f, 0.5f, 0.5f,
		0.5f, 0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, 0.5f, -0.5f,
		0.5f, -0.5f, 0.5f,
		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, 0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, 0.5f, 0.5f,
		-0.5f, 0.5f, -0.5f,
		0.5f, -0.5f, 0.5f,
		-0.5f, -0.5f, 0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, 0.5f, 0.5f,
		-0.5f, -0.5f, 0.5f,
		0.5f, -0.5f, 0.5f,
		0.5f, 0.5f, 0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, 0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, 0.5f, 0.5f,
		0.5f, -0.5f, 0.5f,
		0.5f, 0.5f, 0.5f,
		0.5f, 0.5f, -0.5f,
		-0.5f, 0.5f, -0.5f,
		0.5f, 0.5f, 0.5f,
		-0.5f, 0.5f, -0.5f,
		-0.5f, 0.5f, 0.5f,
		0.5f, 0.5f, 0.5f,
		-0.5f, 0.5f, 0.5f,
		0.5f, -0.5f, 0.5f
	};

	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO
	
	return VAO;
}

GLuint initXAxis(){
	GLfloat axis_vertices[] =
	{
		0.0f, 0.0f, 0.0f,
		5.0f, 0.0f, 0.0f
	};

	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axis_vertices), axis_vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO

	return VAO;
}

GLuint initYAxis(){
	GLfloat axis_vertices[] =
	{
		0.0f, 0.0f, 0.0f,
		0.0f, 5.0f, 0.0f
	};

	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axis_vertices), axis_vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO

	return VAO;
}

GLuint initZAxis(){
	GLfloat axis_vertices[] =
	{
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 5.0f
	};

	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axis_vertices), axis_vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO

	return VAO;
}

GLuint initGrid(){
	//Set grid vertices
	GLfloat grid_vertices[1212];
	int current_row = 0;
	GLfloat temp_coordinate = -50.0f;
	for (int i = 0; i < 1212; i += 12, temp_coordinate += 1.0f){
		//point 1 of the square
		grid_vertices[i] = temp_coordinate;
		grid_vertices[i + 1] = 0.0f;
		grid_vertices[i + 2] = -50.0f;
	
		//point 2 of the line
		grid_vertices[i + 3] = temp_coordinate;
		grid_vertices[i + 4] = 0.0f;
		grid_vertices[i + 5] = 50.0f;
	
		//point 1 of the 2nd line     
		grid_vertices[i + 6] = -50.0f;
		grid_vertices[i + 7] = 0.0f;
		grid_vertices[i + 8] = temp_coordinate;
	
		//point 2 of the 2nd line
		grid_vertices[i + 9] = 50.0f;
		grid_vertices[i + 10] = 0.0f;
		grid_vertices[i + 11] = temp_coordinate;
	}


	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(grid_vertices), grid_vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO

	return VAO;
}


void drawObj(int vertexNum){
	if (mode == 'T'){
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawArrays(GL_TRIANGLES, 0, vertexNum);
	}
	else if (mode == 'P'){
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawArrays(GL_POINTS, 0, vertexNum);
	}
	else if (mode == 'L'){
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawArrays(GL_TRIANGLES, 0, vertexNum);
	}
}

void drawCube(GLuint cubeVAO){
	//rotate *= glm::rotate(glm::mat4(1.0f), 0.f, glm::vec3(1, 0, 0)); //rotation modification around x
	//rotate *= glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0, 1, 0)); //rotation modification around y
	//rotate *= glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0, 0, 1)); //rotation modification around z
	//scale *= glm::scale(glm::mat4(1.0f), scaleMod);	//scaling modifications
	//translate *= glm::translate(glm::mat4(1.0f), translateMod); //translation modification on all 3 axis
	glm::mat4 rotateModMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(rotateMod.x), glm::vec3(1, 0, 0)); //rotate x
	rotateModMatrix *= glm::rotate(glm::mat4(1.0f), glm::radians(rotateMod.y), glm::vec3(0, 1, 0)); //rotate y
	rotateModMatrix *= glm::rotate(glm::mat4(1.0f), glm::radians(rotateMod.z), glm::vec3(0, 0, 1)); //rotate z
	//glm::mat4 scaleModMatrix = glm::scale(glm::mat4(1.0f), scaleMod);
	scale *= glm::scale(glm::mat4(1.0f), scaleMod);	//scaling modifications
	glm::mat4 translateModMatrix = glm::translate(glm::mat4(1.0f), translateMod); //translation modification on all 3 axis
	mm =  translateModMatrix * rotateModMatrix * translate * rotate * scale;
	glUniformMatrix4fv(mm_addr, 1, false, glm::value_ptr(mm));
	glBindVertexArray(cubeVAO);
	drawObj(36);
	glBindVertexArray(0);
}

void drawXAxis(GLuint axisVAO){
	glUniform1i(fillLoc, 3);
	glBindVertexArray(axisVAO);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);
}

void drawYAxis(GLuint axisVAO){
	glUniform1i(fillLoc, 4);
	glBindVertexArray(axisVAO);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);
}

void drawZAxis(GLuint axisVAO){
	glUniform1i(fillLoc, 2);
	glBindVertexArray(axisVAO);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);
}

void drawGrid(GLuint gridVAO){
	glUniform1i(fillLoc, 1);
	glBindVertexArray(gridVAO);
	glDrawArrays(GL_LINES, 0, 202 * 2);
	glBindVertexArray(0);
}

void drawBody(GLuint cubeVAO){  //torso
	glm::vec3 body_pos = glm::vec3(0, body.y / 2 + leg.x * 2, 0) * scaleMod.x;
	scale = glm::scale(glm::mat4(1.0f), glm::vec3( 5, 2, 2)); // 5 2 2
	translate = glm::translate(glm::mat4(1.0f), body_pos);
	//translate = glm::translate(glm::mat4(1.0f), glm::vec3(0, 4, 0));
	rotate = glm::rotate(glm::mat4(1.0f), glm::radians(0.f), glm::vec3(0, 0, 1));
	glUniform1i(fillLoc, 0);
	drawCube(cubeVAO);
}

void drawNeck(GLuint cubeVAO){
	glm::vec3 neck_pos = glm::vec3(-0.55* body.x, 1.125*(body.y / 2 + leg.x * 2), 0) * scaleMod.x;
	scale = glm::scale(glm::mat4(1.0f), glm::vec3(2, 1.25, 1.25));
	rotate = glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(0, 0, 1));
	translate = glm::translate(glm::mat4(1.0f), neck_pos);
	//translate = glm::translate(glm::mat4(1.0f), glm::vec3(-2.75, 4.5, 0));
	glUniform1i(fillLoc, 5);
	drawCube(cubeVAO);
}

void drawHead(GLuint cubeVAO){
	glm::vec3 head_pos = glm::vec3(-0.75* body.x, 1.25*(body.y / 2 + leg.x * 2), 0) * scaleMod.x;
	scale = glm::scale(glm::mat4(1.0f), glm::vec3(2, 1.25, 1.25));
	rotate = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0, 0, 1));
	translate = glm::translate(glm::mat4(1.0f), head_pos);
	//translate = glm::translate(glm::mat4(1.0f), glm::vec3(-3.75, 5, 0)* scaleMod.x) ;
	glUniform1i(fillLoc, 6);
	drawCube(cubeVAO);
}

void drawFrontLeftUpperLeg(GLuint cubeVAO){
	glm::vec3 leg_pos = glm::vec3(-0.5*(body.x - leg.y), 1.5*leg.x , 0.5*body.z - 0.5*leg.z) * scaleMod.x;
	scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.5, 0.5, 0.5));
	rotate = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 0, 1));
	//translate = glm::translate(glm::mat4(1.0f), glm::vec3(-2.25, 2.25, 0.75));
	translate = glm::translate(glm::mat4(1.0f), glm::vec3(leg_pos));
	glUniform1i(fillLoc, 5);
	drawCube(cubeVAO);
}

void drawFrontLeftLowerLeg(GLuint cubeVAO){
	glm::vec3 leg_pos = glm::vec3(-0.5*(body.x - leg.y), 0.5*leg.x, 0.5*body.z - 0.5*leg.z) * scaleMod.x;
	scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.5, 0.5, 0.5));
	rotate = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 0, 1));
	translate = glm::translate(glm::mat4(1.0f), leg_pos);
	//translate = glm::translate(glm::mat4(1.0f), glm::vec3(-2.25, 0.75, 0.75));
	glUniform1i(fillLoc, 6);
	drawCube(cubeVAO);
}

void drawFrontRightUpperLeg(GLuint cubeVAO){
	glm::vec3 leg_pos = glm::vec3(-0.5*(body.x - leg.y), 1.5*leg.x, -(0.5*body.z - 0.5*leg.z)) * scaleMod.x;
	scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.5, 0.5, 0.5));
	rotate = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 0, 1));
	//translate = glm::translate(glm::mat4(1.0f), glm::vec3(-2.25, 2.25, -0.75));
	translate = glm::translate(glm::mat4(1.0f), leg_pos);
	glUniform1i(fillLoc, 5);
	drawCube(cubeVAO);
}

void drawFrontRightLowerLeg(GLuint cubeVAO){
	glm::vec3 leg_pos = glm::vec3(-0.5*(body.x - leg.y), 0.5*leg.x, -(0.5*body.z - 0.5*leg.z)) * scaleMod.x;
	scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.5, 0.5, 0.5));
	rotate = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 0, 1));
//	translate = glm::translate(glm::mat4(1.0f), glm::vec3(-2.25, 0.75, -0.75));
	translate = glm::translate(glm::mat4(1.0f), leg_pos);
	glUniform1i(fillLoc, 6);
	drawCube(cubeVAO);
}

void drawHindLeftUpperLeg(GLuint cubeVAO){
	glm::vec3 leg_pos = glm::vec3(0.5*(body.x - leg.y), 1.5*leg.x, 0.5*body.z - 0.5*leg.z) * scaleMod.x;
	scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.5, 0.5, 0.5));
	rotate = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 0, 1));
	translate = glm::translate(glm::mat4(1.0f), leg_pos);
	//translate = glm::translate(glm::mat4(1.0f), glm::vec3(2.25, 2.25, 0.75));
	glUniform1i(fillLoc, 5);
	drawCube(cubeVAO);
}

void drawHindLeftLowerLeg(GLuint cubeVAO){
	glm::vec3 leg_pos = glm::vec3(0.5*(body.x - leg.y), 0.5*leg.x, 0.5*body.z - 0.5*leg.z) * scaleMod.x;
	scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.5, 0.5, 0.5));
	rotate = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 0, 1));
	translate = glm::translate(glm::mat4(1.0f), leg_pos);
	//translate = glm::translate(glm::mat4(1.0f), glm::vec3(2.25, 0.75, 0.75));
	glUniform1i(fillLoc, 6);
	drawCube(cubeVAO);
}

void drawHindRightUpperLeg(GLuint cubeVAO){
	glm::vec3 leg_pos = glm::vec3(0.5*(body.x - leg.y), 1.5*leg.x, -(0.5*body.z - 0.5*leg.z)) * scaleMod.x;
	scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.5, 0.5, 0.5));
	rotate = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 0, 1));
	translate = glm::translate(glm::mat4(1.0f), leg_pos);
	//translate = glm::translate(glm::mat4(1.0f), glm::vec3(2.25, 2.25, -0.75));
	glUniform1i(fillLoc, 5);
	drawCube(cubeVAO);
}

void drawHindRightLowerLeg(GLuint cubeVAO){
	glm::vec3 leg_pos = glm::vec3(0.5*(body.x - leg.y), 0.5*leg.x, -(0.5*body.z - 0.5*leg.z)) * scaleMod.x;
	scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.5, 0.5, 0.5));
	rotate = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 0, 1));
	translate = glm::translate(glm::mat4(1.0f), leg_pos);
	//translate = glm::translate(glm::mat4(1.0f), glm::vec3(2.25, 0.75, -0.75));
	glUniform1i(fillLoc, 6);
	drawCube(cubeVAO);
}

void drawHorse(GLuint cubeVAO){
	drawBody(cubeVAO);
	drawNeck(cubeVAO);
	drawHead(cubeVAO);
	drawFrontLeftUpperLeg(cubeVAO);
	drawFrontLeftLowerLeg(cubeVAO);

	drawFrontRightUpperLeg(cubeVAO);
	drawFrontRightLowerLeg(cubeVAO);

	drawHindLeftUpperLeg(cubeVAO);
	drawHindLeftLowerLeg(cubeVAO);

	drawHindRightUpperLeg(cubeVAO);
	drawHindRightLowerLeg(cubeVAO);
}

void resetModelMatrix(){
	scale = glm::scale(glm::mat4(1.0f), glm::vec3(1, 1, 1));
	translate = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
	rotate = glm::rotate(glm::mat4(1.0f), glm::radians(0.f), glm::vec3(0, 0, 1));
	mm = translate * rotate * scale;
	glUniformMatrix4fv(mm_addr, 1, false, glm::value_ptr(mm));
}

int main() {

	if (init() != 0) {
		return -1;
	}

	GLuint shdr = loadShaders("v.glsl", "f.glsl");
	glUseProgram(shdr);


	//Component Initialization
	GLuint cubeVAO = initCube();
	GLuint x_axisVAO = initXAxis();
	GLuint y_axisVAO = initYAxis();
	GLuint z_axisVAO = initZAxis();
	GLuint gridVAO = initGrid();

	mm_addr = glGetUniformLocation(shdr, "m_m");
	vm_addr = glGetUniformLocation(shdr, "v_m");
	pm_addr = glGetUniformLocation(shdr, "p_m");
	fillLoc = glGetUniformLocation(shdr, "fill");
	

	glClearColor(0.7f, 0.7f, 0.7f, 0);
	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window)) {


		glfwPollEvents();
		glfwSwapBuffers(window);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
		scale = glm::scale(glm::mat4(1.0f), glm::vec3(1, 1, 1));
		translate = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
		rotate = glm::rotate(glm::mat4(1.0f), glm::radians(0.f), glm::vec3(0, 0, 1));
		mm = translate * rotate * scale;

		glUniformMatrix4fv(mm_addr, 1, false, glm::value_ptr(mm));
		glUniformMatrix4fv(vm_addr, 1, false, glm::value_ptr(vm));
		glUniformMatrix4fv(pm_addr, 1, false, glm::value_ptr(pm));//needed to make sure that they are initialized in the shader (i think?)

		drawGrid(gridVAO);
		drawHorse(cubeVAO);
	
		resetModelMatrix();

		drawXAxis(x_axisVAO);
		drawYAxis(y_axisVAO);
		drawZAxis(z_axisVAO);
		

	}
	return 0;
}

