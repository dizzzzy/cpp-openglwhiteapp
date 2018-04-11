#include "NewGrid.h"

void NewGrid::init(){
	GLfloat grid_vertices[] =
	{
		-50.0f, 0, -50.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		50.0f, 0, -50.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		-50.0f, 0, 50.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,

		50.0f, 0, -50.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		50.0f, 0, 50.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		-50.0f, 0, 50.0f, 0.0, 1.0f, 0.0f, 1.0f, 0.0f
	};

	GLuint VBO;
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &VBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(this->VAO);

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

}

void NewGrid::draw(Shader* shader){
	shader->use();
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(1, 1, 1));
	glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
	glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), glm::radians(0.f), glm::vec3(0, 0, 1));
	glm::mat4 mm = translate * rotate * scale;
	shader->setMm(mm);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	shader->setFillLoc(7);
	glBindVertexArray(this->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}