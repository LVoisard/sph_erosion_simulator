#include "terrain_mesh.h"

TerrainMesh::TerrainMesh(int width, int length, float*** terrainHeights, Shader shader)
	:QuadMesh(width, length, shader)
{
	originalHeights = new float[width * length];
	offset = glm::vec2(width / 2, length / 2);

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
	offset = glm::vec2(width / 2, length / 2);

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

struct CellPosition {
	int xLeft;
	int xRight;
	int yDown;
	int yUp;
	float xWeight;
	float yWeight;

public: 
	CellPosition(float x, float y) {
		// Float casted to int are truncated towards 0.
		xLeft = (int)x;
		xRight = xLeft + 1;
		// This will act as the horizontal weight, indicating how close the point is to one side.
		xWeight = x - xLeft; // must be between [0, 1).

		yDown = (int)y;
		yUp = yDown + 1;
		// This will act as the vertical weight, indicating how close the point is to one side.
		yWeight = y - yDown;  // must be between [0, 1).
	}
};

float TerrainMesh::sampleHeightAtPosition(float x, float y) const {
	// The position we sample lies within a grid cell of our heightmap.
	// We sample the four corners of that cell and return an average weighted
	// by how close the position we sample is to each corner.
	x += offset.x;
	y += offset.y;
	CellPosition cell = CellPosition(x, y);

	// Sample the heightmap at each of the cell's corner.
	if (cell.xLeft < 0 || cell.xLeft >= width || cell.yDown < 0 || cell.yDown >= length) return 0;
	if (cell.xRight < 0 || cell.xRight >= width || cell.yUp < 0 || cell.yUp >= length) return 0;
	float bottomLeftHeight = vertices[cell.yDown * width + cell.xLeft].pos.y;
	float bottomRightHeight = vertices[cell.yDown * width + cell.xRight].pos.y;
	float topLeftHeight = vertices[cell.yUp * width + cell.xLeft].pos.y;
	float topRightHeight = vertices[cell.yUp * width + cell.xRight].pos.y;

	// Adjust the weight of each sample by how close the target position is to it.
	bottomLeftHeight *= (1 - cell.xWeight) * (1 - cell.yWeight);
	bottomRightHeight *= cell.xWeight * (1 - cell.yWeight);
	topLeftHeight *= (1 - cell.xWeight) * (cell.yWeight);
	topRightHeight *= cell.xWeight * cell.yWeight;

	// Return the average.
	return (bottomLeftHeight + bottomRightHeight + topLeftHeight + topRightHeight);
}

void TerrainMesh::modify_height(float x, float y, float amount) {
	x += offset.x;
	y += offset.y;
	CellPosition cell = CellPosition(x, y);

	// Sample the heightmap at each of the cell's corner.
	if (cell.xLeft < 0 || cell.xLeft >= width || cell.yDown < 0 || cell.yDown >= length) return;
	if (cell.xRight < 0 || cell.xRight >= width || cell.yUp < 0 || cell.yUp >= length) return;

	// Adjust the weight of each sample by how close the target position is to it.
	float weights[] = {
		(1 - cell.xWeight) * (1 - cell.yWeight), // bottom-left
		cell.xWeight * (1 - cell.yWeight), // bottom-right
		(1 - cell.xWeight) * (cell.yWeight), // top-left
		cell.xWeight * cell.yWeight // top-right
	};
	Vertex* cellVertices[] = {
		&vertices[cell.yDown * width + cell.xLeft], // bottom-left
		&vertices[cell.yDown * width + cell.xRight], // bottom-right
		&vertices[cell.yUp * width + cell.xLeft], // top-left
		&vertices[cell.yUp * width + cell.xRight] // top-right
	};

	for (int i = 0; i < sizeof(weights) / sizeof(float); i++)
	{
		// Compute the new height for each vertex.
		float newHeight = cellVertices[i]->pos.y + amount * weights[i];

		// Limit the minimum height to 0.
		if (newHeight <= -length) cellVertices[i]->pos.y = -length;
		else cellVertices[i]->pos.y = newHeight;
	}
}

glm::vec3 TerrainMesh::sampleNormalAtPosition(float x, float y) const
{
	// this wont be the exact normal, but can do
	// we could probably just sample the normals of the 4 cell corners this lands in and perform a weighted average, which would be much less expensive than this

	// Float casted to int are truncated towards 0.
	int xLeft = (int)(x - 1);
	int xRight = (int)(x + 1);
	// This will act as the horizontal weight, indicating how close the point is to one side.

	int yDown = (int)(y - 1);
	int yUp = (int)(y + 1);
	// This will act as the vertical weight, indicating how close the point is to one side.

	// Sample the heightmap at each of the cell's corner.


	glm::vec3 self = glm::vec3(x, sampleHeightAtPosition(x, y), y);
	glm::vec3 left = glm::vec3(self);
	glm::vec3 right = glm::vec3(self);
	glm::vec3 top = glm::vec3(self);
	glm::vec3 bottom = glm::vec3(self);

	if (xLeft >= -offset.x)
		left = glm::vec3(xLeft, sampleHeightAtPosition(xLeft, y), y);
	if (xRight <= width + offset.x)
		right = glm::vec3(xRight, sampleHeightAtPosition(xRight, y), y);
	if (yUp <= length + offset.y)
		top = glm::vec3(x, sampleHeightAtPosition(x, yUp), yUp);
	if (yDown >= -offset.y)
		bottom = glm::vec3(x, sampleHeightAtPosition(x, yDown), yDown);

	glm::vec3 normal = glm::cross(glm::normalize(right - left), glm::normalize(top - bottom));

	// Adjust the weight of each sample by how close the target position is to it.


	// Return the average.
	return normal;
}