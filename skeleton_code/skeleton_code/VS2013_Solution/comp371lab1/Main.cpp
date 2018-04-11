// opengl.cpp : Defines the entry point for the console application.
//
//#define GLM_ENABLE_EXPERIMENTAL
#include "stdafx.h"
#include "stb_image.h"
//#include <glm.hpp>
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include <glew.h>
#include <glfw3.h>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

//Component Includes
#include "Shader.h"
#include "LightShader.h"
#include "Grid.h"
#include "NewGrid.h"

Shader* shader;
LightShader* lightShader;

GLFWwindow* window;



glm::vec3 lightColor(1.0);
glm::vec3 l_pos = glm::vec3(0, 20, 0);
glm::vec3 c_pos = glm::vec3(0, 0, 3);
glm::vec3 c_dir = glm::normalize(glm::vec3(0, 0, -3));
glm::vec3 center = glm::vec3(0, 0, 0); //were locking what the camera is looking at.
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
bool x_pressed = false;

//-------CURSOR POSITION UPON CLICK-----
double cursorXPos; 
double cursorYPos;

//---------Joint Rotation Angle--------
glm::fvec1 headToNeck;
glm::fvec1 neckToTorso;
glm::fvec1 torsoToFrontUpperRightLeg;
glm::fvec1 frontRightKnee;
glm::fvec1 torsoToFrontUpperLeftLeg;
glm::fvec1 frontLeftKnee;
glm::fvec1 torsoToHindUpperRightLeg;
glm::fvec1 hindRightKnee;
glm::fvec1 torsoToHindUpperLeftLeg;
glm::fvec1 hindLeftKnee;

//---------Joint Translation---------
glm::fvec3 headToNeckOffset;
glm::fvec3 frontRightKneeOffset;
glm::fvec3 frontLeftKneeOffset;
glm::fvec3 hindRightKneeOffset;
glm::fvec3 hindLeftKneeOffset;

//---------Joint Rotation Matrix--------
glm::mat4 headToNeckMatrix;
glm::mat4 frontRightKneeMatrix;
glm::mat4 frontLeftKneeMatrix;
glm::mat4 hindRightKneeMatrix;
glm::mat4 hindLeftKneeMatrix;


//TESTING PURPOSES
glm::mat4 translateBacktoOrigin;
glm::mat4 jointTransformation;

glm::vec2 worldRotation;


void update_view() {
	vm = glm::lookAt(c_pos, c_pos + c_dir, c_up);
	vm = glm::rotate(vm, glm::radians(worldRotation.x), glm::vec3(1, 0, 0));
	vm = glm::rotate(vm, glm::radians(worldRotation.y), glm::vec3(0, 0, 1));
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

	if (key == GLFW_KEY_X && action == GLFW_PRESS) {
		if (x_pressed){
			x_pressed = false;
		}
		else{
			x_pressed = true;
		}
		std::cout << "x_pressed: " << x_pressed << std::endl;
	}

	if (key == GLFW_KEY_9) {
		if (mods == GLFW_MOD_SHIFT) {
			torsoToHindUpperLeftLeg.x += 5;
		}
		else {
			torsoToHindUpperLeftLeg.x -= 5;
		}
	}

	if (key == GLFW_KEY_8) {
		if (mods == GLFW_MOD_SHIFT) {
			frontLeftKnee.x += 5;
		}
		else {
			frontLeftKnee.x -= 5;
		}
	}

	if (key == GLFW_KEY_7) {
		if (mods == GLFW_MOD_SHIFT) {
			torsoToFrontUpperLeftLeg.x += 5;
		}
		else {
			torsoToFrontUpperLeftLeg.x -= 5;
		}
	}
	
	if (key == GLFW_KEY_6) {
		if (mods == GLFW_MOD_SHIFT) {
			hindRightKnee.x += 5;
		}
		else {
			hindRightKnee.x -= 5;
		}
	}

	if (key == GLFW_KEY_5) {
		if (mods == GLFW_MOD_SHIFT) {
			torsoToHindUpperRightLeg.x += 5;
		}
		else {
			torsoToHindUpperRightLeg.x -= 5;
		}
	}

	if (key == GLFW_KEY_4) {
		if (mods == GLFW_MOD_SHIFT) {
			frontRightKnee.x += 5;
		}
		else {
			frontRightKnee.x -= 5;
		}
	}

	if (key == GLFW_KEY_3) {
		if (mods == GLFW_MOD_SHIFT) {
			torsoToFrontUpperRightLeg.x += 5;
		}
		else {
			torsoToFrontUpperRightLeg.x -= 5;
		}
	}

	if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
		if (mods == GLFW_MOD_SHIFT) {
			neckToTorso.x += 5;
		}
		else {
			neckToTorso.x -= 5;
		}
	}

	if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
		if (mods == GLFW_MOD_SHIFT) {
			headToNeck.x += 5;
		}
		else {
			headToNeck.x -= 5;
		}
	}

	if (key == GLFW_KEY_0 && action == GLFW_PRESS) {
		if (mods == GLFW_MOD_SHIFT) {
			hindLeftKnee.x += 5;
		}
		else {
			hindLeftKnee.x -= 5;
		}
	}

	if (key == GLFW_KEY_0 && action == GLFW_PRESS) {
		if (mods == GLFW_MOD_SHIFT) {
			hindLeftKnee.x += 5;
		}
		else {
			hindLeftKnee.x -= 5;
		}
	}

	if (key == GLFW_KEY_UP) {
		c_pos.y += 0.5;
		update_view();
	}

	if (key == GLFW_KEY_DOWN) {
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

	if (key == GLFW_KEY_H) {
		worldRotation.x += 1;
		update_view();
	}

	if (key == GLFW_KEY_N) {
		worldRotation.x -= 1;
		update_view();
	}

	if (key == GLFW_KEY_HOME) {
		worldRotation.x =0;
		worldRotation.y = 0;
		update_view();
	}

	if (key == GLFW_KEY_B) {
		worldRotation.y += 1;
		update_view();
	}

	if (key == GLFW_KEY_M) {
		worldRotation.y -= 1;
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
		//position				//texture		//normal
		-0.5f, -0.5f, -0.5f,	0.0f, 0.0f,		0.0f, 0.0f, -1.0f,
		0.5f, -0.5f, -0.5f,		1.0f, 0.0f,		0.0f, 0.0f, -1.0f,
		0.5f, 0.5f, -0.5f,		1.0f, 1.0f,		0.0f, 0.0f, -1.0f,
		0.5f, 0.5f, -0.5f,		1.0f, 1.0f,		0.0f, 0.0f, -1.0f,
		-0.5f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,	0.0f, 0.0f,		0.0f, 0.0f, -1.0f,

		-0.5f, -0.5f, 0.5f,		0.0f, 0.0f,		0.0f,  0.0f,  1.0f,
		0.5f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f,  0.0f,  1.0f,
		0.5f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f,  0.0f,  1.0f,
		0.5f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f,  0.0f,  1.0f,
		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f,		0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, 0.5f,		0.0f, 0.0f,		0.0f,  0.0f,  1.0f,

		-0.5f, 0.5f, 0.5f,		1.0f, 0.0f,		-1.0f,  0.0f,  0.0f,
		-0.5f, 0.5f, -0.5f,		1.0f, 1.0f,		-1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,		-1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,		-1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, 0.5f,		0.0f, 0.0f,		-1.0f,  0.0f,  0.0f,
		-0.5f, 0.5f, 0.5f,		1.0f, 0.0f,		-1.0f,  0.0f,  0.0f,

		0.5f, 0.5f, 0.5f,		1.0f, 0.0f,		1.0f,  0.0f,  0.0f,
		0.5f, 0.5f, -0.5f,		1.0f, 1.0f,		1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,		0.0f, 1.0f,		1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,		0.0f, 1.0f,		1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, 0.5f,		0.0f, 0.0f,		1.0f,  0.0f,  0.0f,
		0.5f, 0.5f, 0.5f,		1.0f, 0.0f,		1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,		0.0f, -1.0f,  0.0f,
		0.5f, -0.5f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f,  0.0f,
		0.5f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f,  0.0f,
		0.5f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,		0.0f, -1.0f,  0.0f,

		-0.5f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f,  1.0f,  0.0f,
		0.5f, 0.5f, -0.5f,		1.0f, 1.0f,		0.0f,  1.0f,  0.0f,
		0.5f, 0.5f, 0.5f,		1.0f, 0.0f,		0.0f,  1.0f,  0.0f,
		0.5f, 0.5f, 0.5f,		1.0f, 0.0f,		0.0f,  1.0f,  0.0f,
		-0.5f, 0.5f, 0.5f,		0.0f, 0.0f,		0.0f,  1.0f,  0.0f,
		-0.5f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f,  1.0f,  0.0f
	};

	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

	/*glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0); original
	glEnableVertexAttribArray(0);*/

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

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

GLuint initNewGrid(){
	GLfloat grid_vertices[] =
	{
		-50.0f, 0, -50.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		50.0f, 0, -50.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		-50.0f, 0, 50.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,

		50.0f, 0, -50.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		50.0f, 0, 50.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		-50.0f, 0, 50.0f, 0.0, 1.0f, 0.0f, 1.0f, 0.0f
	};

	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(grid_vertices), grid_vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO

	return VAO;
}

GLuint initLamp(){
	GLfloat lamp_vertices[] = { // 1x1x1 cube centered at (0,0,0)
		//position				//texture		//normal
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,

		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

		-0.5f, 0.5f, 0.5f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,

		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,

		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,

		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f
	};

	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lamp_vertices), lamp_vertices, GL_STATIC_DRAW);

	/*glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0); original
	glEnableVertexAttribArray(0);*/

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO

	return VAO;
}

void drawLamp(GLuint lampVAO){
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBindVertexArray(lampVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
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
	glm::mat4 rotateModMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(rotateMod.x), glm::vec3(1, 0, 0)); //rotate x
	rotateModMatrix *= glm::rotate(glm::mat4(1.0f), glm::radians(rotateMod.y), glm::vec3(0, 1, 0)); //rotate y
	rotateModMatrix *= glm::rotate(glm::mat4(1.0f), glm::radians(rotateMod.z), glm::vec3(0, 0, 1)); //rotate z
	scale *= glm::scale(glm::mat4(1.0f), scaleMod);	//scaling modifications
	glm::mat4 translateModMatrix = glm::translate(glm::mat4(1.0f), translateMod); //translation modification on all 3 axis
	mm = jointTransformation * translateModMatrix * rotateModMatrix * translate * rotate * scale;
	shader->setMm(mm);
	glBindVertexArray(cubeVAO);
	drawObj(36);
	glBindVertexArray(0);
}

void drawXAxis(GLuint axisVAO){
	shader->setFillLoc(3);
	glBindVertexArray(axisVAO);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);
}

void drawYAxis(GLuint axisVAO){
	shader->setFillLoc(4);
	glBindVertexArray(axisVAO);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);
}

void drawZAxis(GLuint axisVAO){
	shader->setFillLoc(2);
	glBindVertexArray(axisVAO);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);
}

void drawNewGrid(GLuint newGridVAO){
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	shader->setFillLoc(7);
	glBindVertexArray(newGridVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void drawBody(GLuint cubeVAO){  //torso
	glm::vec3 body_pos = glm::vec3(0, body.y / 2 + leg.x * 2, 0) * scaleMod.x;
	scale = glm::scale(glm::mat4(1.0f), glm::vec3( 5, 2, 2)); // 5 2 2
	translate = glm::translate(glm::mat4(1.0f), body_pos);
	rotate = glm::rotate(glm::mat4(1.0f), glm::radians(0.f), glm::vec3(0, 0, 1));
	shader->setFillLoc(0);
	drawCube(cubeVAO);
}

void drawNeck(GLuint cubeVAO){
	glm::vec3 neck_pos = glm::vec3(-0.55* body.x, 1.125*(body.y / 2 + leg.x * 2), 0) * scaleMod.x;
	scale = glm::scale(glm::mat4(1.0f), glm::vec3(2, 1.25, 1.25));
	//---------JOINT ROTATION------------
	glm::mat4 jointRotation;
	jointRotation = glm::translate(glm::mat4(1.0f), glm::vec3(-1, 0, 0)); //translate joint down to (0,0,0)
	jointRotation = glm::rotate(glm::mat4(1.0f), glm::radians(neckToTorso.x), glm::vec3(0, 0, 1)) * jointRotation;
	jointRotation = glm::translate(glm::mat4(1.0f), glm::vec3(1, 0, 0)) * jointRotation; //revert translation 
	headToNeckMatrix = jointRotation;
	//---------------------------------
	
	rotate = glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(0, 0, 1));// * jointRotation;
	translate = glm::translate(glm::mat4(1.0f), neck_pos);

	//---------JOINT OFFSET TRANSLATION------
	glm::vec4 joint2Point(0.5, 0, 0, 1);

	glm::mat4 rotateModMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(rotateMod.x), glm::vec3(1, 0, 0)); //rotate x
	rotateModMatrix *= glm::rotate(glm::mat4(1.0f), glm::radians(rotateMod.y), glm::vec3(0, 1, 0)); //rotate y
	rotateModMatrix *= glm::rotate(glm::mat4(1.0f), glm::radians(rotateMod.z), glm::vec3(0, 0, 1)); //rotate z
	glm::mat4 tempScale = scale * glm::scale(glm::mat4(1.0f), scaleMod);	//scaling modifications
	glm::mat4 translateModMatrix = glm::translate(glm::mat4(1.0f), translateMod); //translation modification on all 3 axis
	mm = translateModMatrix * rotateModMatrix * translate * rotate * tempScale;
	joint2Point = mm * joint2Point;
	translateBacktoOrigin = glm::translate(glm::mat4(1.0f), glm::vec3(-joint2Point.x, -joint2Point.y, -joint2Point.z)); //translation modification on all 3 axis
	glm::mat4 translateBacktoLocation = glm::translate(glm::mat4(1.0f), glm::vec3(joint2Point.x, joint2Point.y, joint2Point.z)); //translation modification on all 3 axis
	jointTransformation = translateBacktoLocation * glm::rotate(glm::mat4(1.0f), glm::radians(neckToTorso.x), glm::vec3(0, 0, 1)) * translateBacktoOrigin; //translateBacktoLocation * 
	//---------------------------------------

	//translate = glm::translate(glm::mat4(1.0f), glm::vec3(-2.75, 4.5, 0));
	shader->setFillLoc(5);
	drawCube(cubeVAO);
}

void drawHead(GLuint cubeVAO){
	glm::vec3 head_pos = glm::vec3(-0.75* body.x, 1.25*(body.y / 2 + leg.x * 2), 0) * scaleMod.x;
	scale = glm::scale(glm::mat4(1.0f), glm::vec3(2, 1.25, 1.25));
	rotate =  glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0, 0, 1)); //headToNeck
	//---------JOINT ROTATION------------
	glm::mat4 jointRotation;
	jointRotation = glm::translate(glm::mat4(1.0f), glm::vec3(-0.75, 0, 0)); //translate joint down to (0,0,0)
	jointRotation = glm::rotate(glm::mat4(1.0f), glm::radians(headToNeck.x), glm::vec3(0, 0, 1)) * jointRotation;
	jointRotation = glm::translate(glm::mat4(1.0f), glm::vec3(0.75, 0, 0)) *jointRotation; //revert translation 
	//---------------------------------
	translate = glm::translate(glm::mat4(1.0f), head_pos) * jointRotation;
	shader->setFillLoc(6);
	drawCube(cubeVAO);
}

void drawFrontLeftUpperLeg(GLuint cubeVAO){
	glm::vec3 leg_pos = glm::vec3(-0.5*(body.x - leg.y), 1.5*leg.x , 0.5*body.z - 0.5*leg.z) * scaleMod.x;
	scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.5, 0.5, 0.5));
	rotate = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 0, 1));
	//---------JOINT ROTATION------------
	glm::mat4 jointRotation;
	jointRotation = glm::translate(glm::mat4(1.0f), glm::vec3(0, -0.75, 0)); //translate joint down to (0,0,0)
	jointRotation = glm::rotate(glm::mat4(1.0f), glm::radians(torsoToFrontUpperLeftLeg.x), glm::vec3(0, 0, 1)) * jointRotation;
	jointRotation = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0.75, 0)) *jointRotation; //revert translation 
	frontLeftKneeMatrix = jointRotation;
	//---------------------------------
	//---------JOINT OFFSET TRANSLATION------
	glm::vec4 joint2Offset(0,-1.5,0,0);
	glm::mat4 tempJointRotationMatrix;
	tempJointRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(torsoToFrontUpperLeftLeg.x), glm::vec3(0, 0, 1));
	joint2Offset = tempJointRotationMatrix * joint2Offset;
	joint2Offset = joint2Offset + glm::vec4(0, 1.5, 0, 0);
	frontLeftKneeOffset.x = joint2Offset.x;
	frontLeftKneeOffset.y = joint2Offset.y;
	frontLeftKneeOffset.z = joint2Offset.z;
	//---------------------------------------
	translate = glm::translate(glm::mat4(1.0f), glm::vec3(leg_pos)) * jointRotation;
	shader->setFillLoc(5);
	drawCube(cubeVAO);
}

void drawFrontLeftLowerLeg(GLuint cubeVAO){
	glm::vec3 leg_pos = glm::vec3(-0.5*(body.x - leg.y), 0.5*leg.x, 0.5*body.z - 0.5*leg.z) * scaleMod.x;
	scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.5, 0.5, 0.5));
	rotate = frontLeftKneeMatrix * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 0, 1));
	//---------JOINT ROTATION------------
	glm::mat4 jointRotation;
	jointRotation = glm::translate(glm::mat4(1.0f), glm::vec3(0, -0.75, 0)); //translate joint down to (0,0,0)
	jointRotation = glm::rotate(glm::mat4(1.0f), glm::radians(frontLeftKnee.x), glm::vec3(0, 0, 1)) * jointRotation;
	jointRotation = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0.75, 0)) *jointRotation; //revert translation 
	//---------------------------------
	translate = glm::translate(glm::mat4(1.0f), leg_pos + frontLeftKneeOffset * scaleMod.x) * jointRotation; //+ frontLeftKneeOffset
	shader->setFillLoc(6);
	drawCube(cubeVAO);
}

void drawFrontRightUpperLeg(GLuint cubeVAO){
	glm::vec3 leg_pos = glm::vec3(-0.5*(body.x - leg.y), 1.5*leg.x, -(0.5*body.z - 0.5*leg.z)) * scaleMod.x;
	scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.5, 0.5, 0.5));
	rotate = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 0, 1));

	//---------JOINT ROTATION------------
	glm::mat4 jointRotation;
	jointRotation = glm::translate(glm::mat4(1.0f), glm::vec3(0, -0.75, 0)); //translate joint down to (0,0,0)
	jointRotation = glm::rotate(glm::mat4(1.0f), glm::radians(torsoToFrontUpperRightLeg.x), glm::vec3(0, 0, 1)) * jointRotation;
	jointRotation = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0.75, 0)) *jointRotation; //revert translation 
	frontRightKneeMatrix = jointRotation;
	//---------------------------------
	//---------JOINT OFFSET TRANSLATION------
	glm::vec4 joint2Offset(0, -1.5, 0, 0);
	glm::mat4 tempJointRotationMatrix;
	tempJointRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(torsoToFrontUpperRightLeg.x), glm::vec3(0, 0, 1));
	joint2Offset = tempJointRotationMatrix * joint2Offset;
	joint2Offset = joint2Offset + glm::vec4(0, 1.5, 0, 0);
	frontRightKneeOffset.x = joint2Offset.x;
	frontRightKneeOffset.y = joint2Offset.y;
	frontRightKneeOffset.z = joint2Offset.z;
	//---------------------------------------
	translate = glm::translate(glm::mat4(1.0f), leg_pos) * jointRotation;
	shader->setFillLoc(5);
	drawCube(cubeVAO);
}

void drawFrontRightLowerLeg(GLuint cubeVAO){
	glm::vec3 leg_pos = glm::vec3(-0.5*(body.x - leg.y), 0.5*leg.x, -(0.5*body.z - 0.5*leg.z)) * scaleMod.x;
	scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.5, 0.5, 0.5));
	rotate = frontRightKneeMatrix * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 0, 1));
	//---------JOINT ROTATION------------
	glm::mat4 jointRotation;
	jointRotation = glm::translate(glm::mat4(1.0f), glm::vec3(0, -0.75, 0)); //translate joint down to (0,0,0)
	jointRotation = glm::rotate(glm::mat4(1.0f), glm::radians(frontRightKnee.x), glm::vec3(0, 0, 1)) * jointRotation;
	jointRotation = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0.75, 0)) *jointRotation; //revert translation 
	//---------------------------------
	translate = glm::translate(glm::mat4(1.0f), leg_pos + frontRightKneeOffset * scaleMod.x) * jointRotation;
	shader->setFillLoc(6);
	drawCube(cubeVAO);
}

void drawHindLeftUpperLeg(GLuint cubeVAO){
	glm::vec3 leg_pos = glm::vec3(0.5*(body.x - leg.y), 1.5*leg.x, 0.5*body.z - 0.5*leg.z) * scaleMod.x;
	scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.5, 0.5, 0.5));
	rotate = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 0, 1));
	//---------JOINT ROTATION------------
	glm::mat4 jointRotation;
	jointRotation = glm::translate(glm::mat4(1.0f), glm::vec3(0, -0.75, 0)); //translate joint down to (0,0,0)
	jointRotation = glm::rotate(glm::mat4(1.0f), glm::radians(torsoToHindUpperLeftLeg.x), glm::vec3(0, 0, 1)) * jointRotation;
	jointRotation = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0.75, 0)) *jointRotation; //revert translation 
	hindLeftKneeMatrix = jointRotation;
	//---------------------------------
	//---------JOINT OFFSET TRANSLATION------
	glm::vec4 joint2Offset(0, -1.5, 0, 0);
	glm::mat4 tempJointRotationMatrix;
	tempJointRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(torsoToHindUpperLeftLeg.x), glm::vec3(0, 0, 1));
	joint2Offset = tempJointRotationMatrix * joint2Offset;
	joint2Offset = joint2Offset + glm::vec4(0, 1.5, 0, 0);
	hindLeftKneeOffset.x = joint2Offset.x;
	hindLeftKneeOffset.y = joint2Offset.y;
	hindLeftKneeOffset.z = joint2Offset.z;
	//---------------------------------------
	translate = glm::translate(glm::mat4(1.0f), leg_pos) * jointRotation;
	shader->setFillLoc(5);
	drawCube(cubeVAO);
}

void drawHindLeftLowerLeg(GLuint cubeVAO){
	glm::vec3 leg_pos = glm::vec3(0.5*(body.x - leg.y), 0.5*leg.x, 0.5*body.z - 0.5*leg.z) * scaleMod.x;
	scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.5, 0.5, 0.5));
	rotate = hindLeftKneeMatrix * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 0, 1));
	//---------JOINT ROTATION------------
	glm::mat4 jointRotation;
	jointRotation = glm::translate(glm::mat4(1.0f), glm::vec3(0, -0.75, 0)); //translate joint down to (0,0,0)
	jointRotation = glm::rotate(glm::mat4(1.0f), glm::radians(hindLeftKnee.x), glm::vec3(0, 0, 1)) * jointRotation;
	jointRotation = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0.75, 0)) *jointRotation; //revert translation 
	//---------------------------------
	translate = glm::translate(glm::mat4(1.0f), leg_pos + hindLeftKneeOffset * scaleMod.x) * jointRotation;
	shader->setFillLoc(6);
	drawCube(cubeVAO);
}

void drawHindRightUpperLeg(GLuint cubeVAO){
	glm::vec3 leg_pos = glm::vec3(0.5*(body.x - leg.y), 1.5*leg.x, -(0.5*body.z - 0.5*leg.z)) * scaleMod.x;
	scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.5, 0.5, 0.5));
	rotate = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 0, 1));
	//---------JOINT ROTATION------------
	glm::mat4 jointRotation;
	jointRotation = glm::translate(glm::mat4(1.0f), glm::vec3(0, -0.75, 0)); //translate joint down to (0,0,0)
	jointRotation = glm::rotate(glm::mat4(1.0f), glm::radians(torsoToHindUpperRightLeg.x), glm::vec3(0, 0, 1)) * jointRotation;
	jointRotation = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0.75, 0)) *jointRotation; //revert translation 
	hindRightKneeMatrix = jointRotation;
	//---------------------------------
	//---------JOINT OFFSET TRANSLATION------
	glm::vec4 joint2Offset(0, -1.5, 0, 0);
	glm::mat4 tempJointRotationMatrix;
	tempJointRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(torsoToHindUpperRightLeg.x), glm::vec3(0, 0, 1));
	joint2Offset = tempJointRotationMatrix * joint2Offset;
	joint2Offset = joint2Offset + glm::vec4(0, 1.5, 0, 0);
	hindRightKneeOffset.x = joint2Offset.x;
	hindRightKneeOffset.y = joint2Offset.y;
	hindRightKneeOffset.z = joint2Offset.z;
	//---------------------------------------
	translate = glm::translate(glm::mat4(1.0f), leg_pos)* jointRotation;
	shader->setFillLoc(5);
	drawCube(cubeVAO);
}

void drawHindRightLowerLeg(GLuint cubeVAO){
	glm::vec3 leg_pos = glm::vec3(0.5*(body.x - leg.y), 0.5*leg.x, -(0.5*body.z - 0.5*leg.z)) * scaleMod.x;
	scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.5, 0.5, 0.5));
	rotate = hindRightKneeMatrix * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 0, 1));
	//---------JOINT ROTATION------------
	glm::mat4 jointRotation;
	jointRotation = glm::translate(glm::mat4(1.0f), glm::vec3(0, -0.75, 0)); //translate joint down to (0,0,0)
	jointRotation = glm::rotate(glm::mat4(1.0f), glm::radians(hindRightKnee.x), glm::vec3(0, 0, 1)) * jointRotation;
	jointRotation = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0.75, 0)) *jointRotation; //revert translation 
	//---------------------------------
	translate = glm::translate(glm::mat4(1.0f), leg_pos + hindRightKneeOffset * scaleMod.x) * jointRotation;
	shader->setFillLoc(6);
	drawCube(cubeVAO);
}

void drawHorse(GLuint cubeVAO){
	jointTransformation = glm::mat4(1.0f);
	drawBody(cubeVAO);
	jointTransformation = glm::mat4(1.0f);
	drawNeck(cubeVAO);
	drawHead(cubeVAO);
	jointTransformation = glm::mat4(1.0f);
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
	shader->setMm(mm);

	//glUniformMatrix4fv(mm_addr, 1, false, glm::value_ptr(mm));
}

void loadDepthMap(){
	const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	GLuint depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	// create depth texture
	GLuint depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

int main() {

	if (init() != 0) {
		return -1;
	}

	shader = new Shader("v.glsl", "f.glsl");

	lightShader = new LightShader("v_lamp.glsl", "f_lamp.glsl");

	shader->use();
	shader->setLightColor(lightColor);
	shader->setLightPos(l_pos);
	shader->setViewPos(c_pos);

	//Component Declaration
	Grid grid;
	NewGrid newGrid;

	//Component Initialization
	grid.init();
	newGrid.init();
	GLuint cubeVAO = initCube();
	GLuint lampVAO = initLamp();
	GLuint x_axisVAO = initXAxis();
	GLuint y_axisVAO = initYAxis();
	GLuint z_axisVAO = initZAxis();
	GLuint newGridVAO = initNewGrid();
	shader->loadTexture0("horse_skin.jpg");
	shader->loadTexture1("grass.jpg");

	loadDepthMap();
	
	glClearColor(0.7f, 0.7f, 0.7f, 0);
	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window)) {
		shader->use();

		glfwPollEvents();
		glfwSwapBuffers(window);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		
	
		scale = glm::scale(glm::mat4(1.0f), glm::vec3(1, 1, 1));
		translate = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
		rotate = glm::rotate(glm::mat4(1.0f), glm::radians(0.f), glm::vec3(0, 0, 1));
		mm = translate * rotate * scale;
		shader->setMm(mm);
		shader->setVm(vm);
		shader->setPm(pm);

		if (x_pressed){
			shader->setX(1);

			
			shader->activateTexture1();
			newGrid.draw(shader);
			//drawNewGrid(newGridVAO);
			shader->UnbindTexture();


			shader->activateTexture0();
			drawHorse(cubeVAO);
			shader->UnbindTexture();

		}
		else{
			shader->setX(0);
			grid.draw(shader);
			drawHorse(cubeVAO);
		}

		resetModelMatrix();

		drawXAxis(x_axisVAO);
		drawYAxis(y_axisVAO);
		drawZAxis(z_axisVAO);

		lightShader->use();
		lightShader->setVm(vm);
		lightShader->setPm(pm);
		glm::mat4 model = glm::mat4();
		model = glm::translate(model, l_pos);
		model = glm::scale(model, glm::vec3(0.2f));
		lightShader->setMm(model);
		drawLamp(lampVAO);

	}
	return 0;
}

//Need to load 20 horses
	//I can instance them 
		//how would i handle movements?
			//i would have to make a seperate model for my each horse
			//the movemetns have to be a vector connected a 
	//I can do a for loop and launch a 