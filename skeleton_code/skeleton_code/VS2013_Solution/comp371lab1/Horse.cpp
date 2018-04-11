#include "Horse.h"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

//void Horse::drawObj(int vertexNum){
//	if (mode == 'T'){
//		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//		glDrawArrays(GL_TRIANGLES, 0, vertexNum);
//	}
//	else if (mode == 'P'){
//		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//		glDrawArrays(GL_POINTS, 0, vertexNum);
//	}
//	else if (mode == 'L'){
//		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//		glDrawArrays(GL_TRIANGLES, 0, vertexNum);
//	}
//}
//
//void Horse::drawCube(GLuint cubeVAO){
//	glm::mat4 rotateModMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(rotateMod.x), glm::vec3(1, 0, 0)); //rotate x
//	rotateModMatrix *= glm::rotate(glm::mat4(1.0f), glm::radians(rotateMod.y), glm::vec3(0, 1, 0)); //rotate y
//	rotateModMatrix *= glm::rotate(glm::mat4(1.0f), glm::radians(rotateMod.z), glm::vec3(0, 0, 1)); //rotate z
//	scale *= glm::scale(glm::mat4(1.0f), scaleMod);	//scaling modifications
//	glm::mat4 translateModMatrix = glm::translate(glm::mat4(1.0f), translateMod); //translation modification on all 3 axis
//	mm = jointTransformation * translateModMatrix * rotateModMatrix * translate * rotate * scale;
//	glUniformMatrix4fv(mm_addr, 1, false, glm::value_ptr(mm));
//	glBindVertexArray(cubeVAO);
//	drawObj(36);
//	glBindVertexArray(0);
//}