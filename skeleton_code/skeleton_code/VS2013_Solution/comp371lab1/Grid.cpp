#include "Grid.h"


void Grid::init(){
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


	GLuint VBO;
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &VBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(this->VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(grid_vertices), grid_vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO
}

void Grid::draw(Shader* shader){
	shader->use();
	shader->setFillLoc(1);
	glBindVertexArray(this->VAO);
	glDrawArrays(GL_LINES, 0, 202 * 2);
	glBindVertexArray(0);
}