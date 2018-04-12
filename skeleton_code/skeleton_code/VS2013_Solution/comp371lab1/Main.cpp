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
#include "Axes.h"
#include "Lamp.h"
#include "Horse.h"


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
	zoom = c_dir * 1.0f;
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
		zoom = c_dir * 0.5f;
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
		Horse::mode = 'T';
		//mode = 'T';
	}
	
	if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		Horse::mode = 'P';
		//mode = 'P';
	}

	if (key == GLFW_KEY_L && action == GLFW_PRESS) {
		Horse::mode = 'L';
		//mode = 'L';
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
	//glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
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
	Axes axes;
	Lamp lamp;
	//Horse horse;
	std::vector<Horse*> horses;
	for (int i = 0; i < 20; i++){
		Horse* horse = new Horse(i);
		horses.push_back(horse);
	}
	

	//Component Initialization
	grid.init();
	newGrid.init();
	axes.init();
	lamp.init();
	for (int i = 0; i < horses.size(); i++){
		horses.at(i)->init();
		horses.at(i)->setShader(shader);
	}
	/*horse.init();
	horse.setShader(shader);*/
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

		
		for (int i = 0; i < horses.size(); i++){
			horses.at(i)->scale = glm::scale(glm::mat4(1.0f), glm::vec3(1, 1, 1));
			horses.at(i)->translate = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
			horses.at(i)->rotate = glm::rotate(glm::mat4(1.0f), glm::radians(0.f), glm::vec3(0, 0, 1));
			horses.at(i)->mm = translate * rotate * scale;
		}
		/*horse.scale = glm::scale(glm::mat4(1.0f), glm::vec3(1, 1, 1));
		horse.translate = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
		horse.rotate = glm::rotate(glm::mat4(1.0f), glm::radians(0.f), glm::vec3(0, 0, 1));
		horse.mm = translate * rotate * scale;*/


		shader->setVm(vm);
		shader->setPm(pm);

		if (x_pressed){
			shader->setX(1);

			
			shader->activateTexture1();
			newGrid.draw(shader);
			shader->UnbindTexture();


			shader->activateTexture0();
			for (int i = 0; i < horses.size(); i++){
				horses.at(i)->draw();
			}
			//horse.draw();
			shader->UnbindTexture();

		}
		else{
			shader->setX(0);
			grid.draw(shader);
			for (int i = 0; i < horses.size()-1; i++){
				for (int j = i +1; j < horses.size(); j++){
					if (!(horses.at(i)->bodyCapsule == nullptr || horses.at(j)->bodyCapsule == nullptr)){
						bool intersected = Capsule::intersect(*(horses.at(i)->bodyCapsule), *(horses.at(j)->bodyCapsule));
						if (intersected){
								horses.at(i)->collided = true;
								horses.at(j)->collided = true;
						}
					}
				}
			}
			for (int i = 0; i < horses.size(); i++){
				horses.at(i)->move();
				horses.at(i)->draw();
				//reset collision 
				horses.at(i)->collided = false;
			}
			//horse.draw();
		}

		axes.draw(shader);

		lightShader->use();
		lightShader->setVm(vm);
		lightShader->setPm(pm);
		lamp.draw(lightShader, l_pos);


	}
	return 0;
}

//Need to load 20 horses
	//I can instance them 
		//how would i handle movements?
			//i would have to make a seperate model for my each horse
			//the movemetns have to be a vector connected a 
	//I can do a for loop and launch a 