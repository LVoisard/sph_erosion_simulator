#include "mesh.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

Mesh::Mesh(Shader& shader):
	shader(shader)
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &VBO);
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, Shader& shader)
	: vertices(vertices), indices(indices), shader(shader)
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &VBO);

	init();
}

Mesh::~Mesh()
{
	clearData();

	if (EBO != 0)
	{
		glDeleteBuffers(1, &EBO);
		EBO = 0;
	}

	if (VBO != 0)
	{
		glDeleteBuffers(1, &VBO);
		VBO = 0;
	}

	if (VAO != 0)
	{
		glDeleteVertexArrays(1, &VAO);
		VAO = 0;
	}
}

void Mesh::init()
{
	glBindVertexArray(VAO);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), indices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(shader.getAttribLocation("pos"));
	glEnableVertexAttribArray(shader.getAttribLocation("normal"));
	glEnableVertexAttribArray(shader.getAttribLocation("uv"));
	glVertexAttribPointer(shader.getAttribLocation("pos"), 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (const GLvoid*)(0));
	glVertexAttribPointer(shader.getAttribLocation("normal"), 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (const GLvoid*)(sizeof(vertices[0].pos)));
	glVertexAttribPointer(shader.getAttribLocation("uv"), 2, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (const GLvoid*)(sizeof(vertices[0].pos) + sizeof(vertices[0].normal)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Mesh::draw()
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Mesh::drawInstanced(int amount)
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, amount);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Mesh::update()
{
	glBindBuffer(GL_ARRAY_BUFFER, VBO);	
		void* data = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		memcpy(data, vertices.data(), sizeof(vertices[0]) * vertices.size());
		glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::clearData()
{
	vertices.clear();
	indices.clear();
}
