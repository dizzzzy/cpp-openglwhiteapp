#include "Axes.h"

void Axes::init(){

	GLfloat xaxis_vertices[] =
	{
		0.0f, 0.0f, 0.0f,
		5.0f, 0.0f, 0.0f
	};

	GLuint xVBO;
	glGenVertexArrays(1, &this->xVAO);
	glGenBuffers(1, &xVBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(this->xVAO);

	glBindBuffer(GL_ARRAY_BUFFER, xVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(xaxis_vertices), xaxis_vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO

	GLfloat yaxis_vertices[] =
	{
		0.0f, 0.0f, 0.0f,
		0.0f, 5.0f, 0.0f
	};

	GLuint yVBO;
	glGenVertexArrays(1, &this->yVAO);
	glGenBuffers(1, &yVBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(this->yVAO);

	glBindBuffer(GL_ARRAY_BUFFER, yVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(yaxis_vertices), yaxis_vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO

	GLfloat zaxis_vertices[] =
	{
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 5.0f
	};

	GLuint zVBO;
	glGenVertexArrays(1, &this->zVAO);
	glGenBuffers(1, &zVBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(this->zVAO);

	glBindBuffer(GL_ARRAY_BUFFER, zVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(zaxis_vertices), zaxis_vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO
}

void Axes::draw(Shader* shader){
	shader->use();

	//Draw x axis
	shader->setFillLoc(3);
	glBindVertexArray(this->xVAO);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);

	//Draw y axis
	shader->setFillLoc(4);
	glBindVertexArray(this->yVAO);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);

	//Draw z axis
	shader->setFillLoc(2);
	glBindVertexArray(this->zVAO);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);
}