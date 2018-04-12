#include "Horse.h"
#include <time.h>
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include <gl/GLU.h>

char Horse::mode = 'L'; 

Horse::Horse(){
}

Horse::Horse(int i){
	srand((i*8)^2);
	//srand(time(NULL));
	int random = rand() % 100 + (-49);
	translateMod.z = random;
	srand(rand()+ i*8);
	int rand2 = rand() % 100 + (-49);
	translateMod.x = rand2;
}

void Horse::drawObj(int vertexNum){
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

void Horse::drawCube(){
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

void Horse::drawBody(){  //torso
	glm::vec3 body_pos = glm::vec3(0, body.y / 2 + leg.x * 2, 0) * scaleMod.x;
	scale = glm::scale(glm::mat4(1.0f), glm::vec3(5, 2, 2)); // 5 2 2
	translate = glm::translate(glm::mat4(1.0f), body_pos);
	rotate = glm::rotate(glm::mat4(1.0f), glm::radians(0.f), glm::vec3(0, 0, 1));
	shader->setFillLoc(0);
	drawCube();
}

void Horse::drawNeck(){
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
	drawCube();
}

void Horse::drawHead(){
	glm::vec3 head_pos = glm::vec3(-0.75* body.x, 1.25*(body.y / 2 + leg.x * 2), 0) * scaleMod.x;
	scale = glm::scale(glm::mat4(1.0f), glm::vec3(2, 1.25, 1.25));
	rotate = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0, 0, 1)); //headToNeck
	//---------JOINT ROTATION------------
	glm::mat4 jointRotation;
	jointRotation = glm::translate(glm::mat4(1.0f), glm::vec3(-0.75, 0, 0)); //translate joint down to (0,0,0)
	jointRotation = glm::rotate(glm::mat4(1.0f), glm::radians(headToNeck.x), glm::vec3(0, 0, 1)) * jointRotation;
	jointRotation = glm::translate(glm::mat4(1.0f), glm::vec3(0.75, 0, 0)) *jointRotation; //revert translation 
	//---------------------------------
	translate = glm::translate(glm::mat4(1.0f), head_pos) * jointRotation;
	shader->setFillLoc(6);
	drawCube();
}

void Horse::drawFrontLeftUpperLeg(){
	glm::vec3 leg_pos = glm::vec3(-0.5*(body.x - leg.y), 1.5*leg.x, 0.5*body.z - 0.5*leg.z) * scaleMod.x;
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
	glm::vec4 joint2Offset(0, -1.5, 0, 0);
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
	drawCube();
}

void Horse::drawFrontLeftLowerLeg(){
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
	drawCube();
}

void Horse::drawFrontRightUpperLeg(){
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
	drawCube();
}

void Horse::drawFrontRightLowerLeg(){
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
	drawCube();
}

void Horse::drawHindLeftUpperLeg(){
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
	drawCube();
}

void Horse::drawHindLeftLowerLeg(){
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
	drawCube();
}

void Horse::drawHindRightUpperLeg(){
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
	drawCube();
}

void Horse::drawHindRightLowerLeg(){
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
	drawCube();
}

void Horse::drawBoundingCapsule(){
	if (!collided){
		shader->setFillLoc(1);
	}
	else{
		shader->setFillLoc(2);
	}

	//glm::vec3 body_pos = glm::vec3(-1, body.y / 2 + leg.x * 2 , 0) * scaleMod.x;
	glm::mat4 translateToCenter = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -0.5));
	rotate = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 1, 0));
	glm::vec3 body_pos = glm::vec3(0, body.y / 2 + leg.x * 2, 0) * scaleMod.x;
	scale = glm::scale(glm::mat4(1.0f), glm::vec3(3.5, 1.42, 1.42)); // 5 2 2
	translate = glm::translate(glm::mat4(1.0f), body_pos);
	
	glm::mat4 rotateModMatrixCyl = glm::rotate(glm::mat4(1.0f), glm::radians(rotateMod.x), glm::vec3(1, 0, 0)); //rotate x
	rotateModMatrixCyl *= glm::rotate(glm::mat4(1.0f), glm::radians(rotateMod.y), glm::vec3(0, 1, 0)); //rotate y
	rotateModMatrixCyl *= glm::rotate(glm::mat4(1.0f), glm::radians(rotateMod.z), glm::vec3(0, 0, 1)); //rotate z
	scale *= glm::scale(glm::mat4(1.0f), scaleMod);	//scaling modifications
	glm::mat4 translateModMatrixCyl = glm::translate(glm::mat4(1.0f), translateMod); //translation modification on all 3 axis
	mm = jointTransformation * translateModMatrixCyl * rotateModMatrixCyl * translate  * scale * rotate * translateToCenter;
	shader->setMm(mm);

	GLUquadricObj* cylinder = gluNewQuadric();
	gluCylinder(cylinder, 1, 1, 1, 16, 16);



	rotate = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0, 1, 0));
	glm::vec3 sphere_pos = glm::vec3(1.75, body.y / 2 + leg.x * 2, 0) * scaleMod.x;
	scale = glm::scale(glm::mat4(1.0f), glm::vec3(1, 1, 1)); // 5 2 2
	translate = glm::translate(glm::mat4(1.0f), sphere_pos);

	glm::mat4 rotateModMatrixSphere1 = glm::rotate(glm::mat4(1.0f), glm::radians(rotateMod.x), glm::vec3(1, 0, 0)); //rotate x
	rotateModMatrixSphere1 *= glm::rotate(glm::mat4(1.0f), glm::radians(rotateMod.y), glm::vec3(0, 1, 0)); //rotate y
	rotateModMatrixSphere1 *= glm::rotate(glm::mat4(1.0f), glm::radians(rotateMod.z), glm::vec3(0, 0, 1)); //rotate z
	scale *= glm::scale(glm::mat4(1.0f), scaleMod);	//scaling modifications
	glm::mat4 translateModMatrixSphere1 = glm::translate(glm::mat4(1.0f), translateMod); //translation modification on all 3 axis
	mm = jointTransformation * translateModMatrixSphere1 * rotateModMatrixSphere1 * translate  * scale * rotate;
	shader->setMm(mm);

	GLUquadricObj* sphere1 = gluNewQuadric();
	gluSphere(sphere1, 1.42, 16, 16);


	rotate = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0, 1, 0));
	glm::vec3 sphere_pos2 = glm::vec3(-1.75, body.y / 2 + leg.x * 2, 0) * scaleMod.x;
	scale = glm::scale(glm::mat4(1.0f), glm::vec3(1, 1, 1)); // 5 2 2
	translate = glm::translate(glm::mat4(1.0f), sphere_pos2);

	glm::mat4 rotateModMatrixSphere = glm::rotate(glm::mat4(1.0f), glm::radians(rotateMod.x), glm::vec3(1, 0, 0)); //rotate x
	rotateModMatrixSphere *= glm::rotate(glm::mat4(1.0f), glm::radians(rotateMod.y), glm::vec3(0, 1, 0)); //rotate y
	rotateModMatrixSphere *= glm::rotate(glm::mat4(1.0f), glm::radians(rotateMod.z), glm::vec3(0, 0, 1)); //rotate z
	scale *= glm::scale(glm::mat4(1.0f), scaleMod);	//scaling modifications
	glm::mat4 translateModMatrixSphere = glm::translate(glm::mat4(1.0f), translateMod); //translation modification on all 3 axis
	mm = jointTransformation * translateModMatrixSphere * rotateModMatrixSphere * translate  * scale * rotate;
	shader->setMm(mm);

	GLUquadricObj* sphere2 = gluNewQuadric();
	gluSphere(sphere2, 1.42, 16, 16);

	Point p1(-1.75 + translateMod.x, body.y / 2 + leg.x * 2 + translateMod.y, 0 + translateMod.z);
	Point p2(1.75 + translateMod.x, body.y / 2 + leg.x * 2 + translateMod.y, 0 + translateMod.z);
	bodyCapsule = new Capsule(1.42, p1, p2);

}

void Horse::draw(){
	shader->use();
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(1, 1, 1));
	glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
	glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), glm::radians(0.f), glm::vec3(0, 0, 1));
	glm::mat4 mm = translate * rotate * scale;
	shader->setMm(mm);
	
	
	jointTransformation = glm::mat4(1.0f);
	drawBody();
	drawBoundingCapsule();
	
	jointTransformation = glm::mat4(1.0f);
	drawNeck();
	drawHead();
	jointTransformation = glm::mat4(1.0f);
	drawFrontLeftUpperLeg();
	drawFrontLeftLowerLeg();
	drawFrontRightUpperLeg();
	drawFrontRightLowerLeg();

	drawHindLeftUpperLeg();
	drawHindLeftLowerLeg();

	drawHindRightUpperLeg();
	drawHindRightLowerLeg();
	
}

void Horse::setShader(Shader* shdr){
	this->shader = shdr;
}

void Horse::init(){
	GLfloat cube_vertices[] = { // 1x1x1 cube centered at (0,0,0)
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

	GLuint VBO;
	glGenVertexArrays(1, &this->cubeVAO);
	glGenBuffers(1, &VBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(this->cubeVAO);

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
}