#ifndef HORSE_H
#define HORSE_H
#include <glm.hpp>
#include <glew.h>

class Horse{
	//----------CONSTRUCTOR------------
	Horse(GLuint);

	//------ MODEL MATRIX ---------
	glm::mat4 mm;
	glm::mat4 scale;
	glm::mat4 translate;
	glm::mat4 rotate;

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
	glm::mat4 jointTransformation;

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

	void drawObj(int vertexNum);
	void drawCube(GLuint cubeVAO);
	void drawBody(GLuint cubeVAO);
	void drawNeck(GLuint cubeVAO);
	void drawHead(GLuint cubeVAO);
	void drawFrontLeftUpperLeg(GLuint cubeVAO);
	void drawFrontLeftLowerLeg(GLuint cubeVAO);
	void drawFrontRightUpperLeg(GLuint cubeVAO);
	void drawFrontRightLowerLeg(GLuint cubeVAO);
	void drawHindLeftUpperLeg(GLuint cubeVAO);
	void drawHindLeftLowerLeg(GLuint cubeVAO);
	void drawHindRightUpperLeg(GLuint cubeVAO);
	void drawHindRightLowerLeg(GLuint cubeVAO);
	void draw();
	void run();
	void walk();

};
#endif