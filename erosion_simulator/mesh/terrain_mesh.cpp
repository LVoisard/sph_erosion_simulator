#include "terrain_mesh.h"

TerrainMesh::TerrainMesh(int width, int length, float*** terrainHeights, Shader shader)
	:QuadMesh(width, length, shader)
{
	originalHeights = new float[width * length];

	for (int y = 0; y < length; y++)
	{
		for (int x = 0; x < width; x++)
		{
			originalHeights[y * width + x] = (*terrainHeights)[x][y];
		}
	}

	calculateVertices(terrainHeights);
	updateOriginalHeights();
	calculateIndices();
	calculateNormals();
}

TerrainMesh::TerrainMesh(int width, int length, HeightMap* heightMap, Shader shader)
	:QuadMesh(width, length, shader)
{
	originalHeights = new float[width * length];

	for (int y = 0; y < length; y++)
	{
		for (int x = 0; x < width; x++)
		{
			originalHeights[y * length + x] = heightMap->sampleHeightAtIndex(x,y);
		}
	}

	calculateVertices(heightMap); 
	updateOriginalHeights();
	calculateIndices();
	calculateNormals();
}

TerrainMesh::~TerrainMesh()
{
}

void TerrainMesh::updateMeshFromHeights(float*** heights)
{
	clearData();
	calculateVertices(heights);
	updateOriginalHeights();
	calculateIndices();
	calculateNormals();
	update();
}

void TerrainMesh::updateOriginalHeights()
{
	for (int y = 0; y < length; y++)
	{
		for (int x = 0; x < width; x++)
		{
			vertices[y * width + x].height = originalHeights[y * width + x];
		}
	}
}

void TerrainMesh::updateOriginalHeights(float*** heights)
{
	for (int y = 0; y < length; y++)
	{
		for (int x = 0; x < width; x++)
		{
			originalHeights[y * length + x] = (*heights)[x][y];
		}
	}
}

void TerrainMesh::init()
{
	glBindVertexArray(VAO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), indices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(shader.getAttribLocation("pos"));
	glEnableVertexAttribArray(shader.getAttribLocation("normal"));
	glEnableVertexAttribArray(shader.getAttribLocation("uv"));
	glEnableVertexAttribArray(shader.getAttribLocation("originalHeight"));
	glVertexAttribPointer(shader.getAttribLocation("pos"), 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (const GLvoid*)(0));
	glVertexAttribPointer(shader.getAttribLocation("normal"), 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (const GLvoid*)(sizeof(vertices[0].pos)));
	glVertexAttribPointer(shader.getAttribLocation("uv"), 2, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (const GLvoid*)(sizeof(vertices[0].pos) + sizeof(vertices[0].normal)));
	glVertexAttribPointer(shader.getAttribLocation("originalHeight"), 1, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (const GLvoid*)(sizeof(vertices[0].pos) + sizeof(vertices[0].normal) + sizeof(vertices[0].uv)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

glm::vec3 TerrainMesh::getNormalAtIndex(int x, int y)
{
	return vertices[y * width + x].normal;
}

glm::vec3 TerrainMesh::getPositionAtIndex(int x, int y)
{
	return vertices[y * width + x].pos;
}
