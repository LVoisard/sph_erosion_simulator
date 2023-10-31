#include "grid_3d.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Grid3D::Grid3D()
{

}

Grid3D::Grid3D(int width, int length, int height, float cellSize, std::vector<Particle*> particles, Shader& shader)
	:width(width), length(length), height(height), cellSize(cellSize), shader(shader)
{
	cells = new Cell***[width];
	for (int x = 0; x < width; x++)
	{
		cells[x] = new Cell**[height];
		for (int y = 0; y < height; y++)
		{
			cells[x][y] = new Cell*[length];
			for (int z = 0; z < length; z++)
			{
				cells[x][y][z] = new Cell(x,y,z, glm::vec3(x - (float)width / 2, y - (float)height / 2, z - (float)length / 2), cellSize, false, shader);
			}
		}
	}

	for (int i = 0; i < particles.size(); i++)
	{
		Cell* cell = getCellFromPosition(particles[i]->getPosition());
		if (cell != nullptr)
			cell->particles.push_back(particles[i]);
	}

	std::vector<Vertex> vertices = {

			Vertex(glm::vec3(0, cellSize, 0), glm::vec3(0.0f,0.0f,0.0f), glm::vec2(0.0f,0.0f)), // 0
			Vertex(glm::vec3(0, 0, 0), glm::vec3(0.0f,0.0f,0.0f), glm::vec2(0.0f,0.0f)), // 1
			Vertex(glm::vec3(cellSize, cellSize, 0), glm::vec3(0.0f,0.0f,0.0f), glm::vec2(0.0f,0.0f)), // 2
			Vertex(glm::vec3(cellSize, 0, 0), glm::vec3(0.0f,0.0f,0.0f), glm::vec2(0.0f,0.0f)), // 3

			Vertex(glm::vec3(0, cellSize, cellSize), glm::vec3(0.0f,0.0f,0.0f), glm::vec2(0.0f,0.0f)), // 4
			Vertex(glm::vec3(cellSize, cellSize, cellSize), glm::vec3(0.0f,0.0f,0.0f), glm::vec2(0.0f,0.0f)), // 5
			Vertex(glm::vec3(0, 0, cellSize), glm::vec3(0.0f,0.0f,0.0f), glm::vec2(0.0f,0.0f)), // 6
			Vertex(glm::vec3(cellSize, 0, cellSize), glm::vec3(0.0f,0.0f,0.0f), glm::vec2(0.0f,0.0f)), // 7
			//-1.0f,  1.0f, -1.0f, // 0
			//-1.0f, -1.0f, -1.0f, // 1
			// 1.0f, -1.0f, -1.0f, // 2
			// 1.0f,  1.0f, -1.0f, // 3

			//-1.0f,  1.0f,  1.0f, // 4
			//-1.0f, -1.0f,  1.0f, // 5
			// 1.0f, -1.0f,  1.0f, // 6
			// 1.0f,  1.0f,  1.0f, // 7
	};

	std::vector<uint32_t> indices = {
		// front
		0, 1, 2,
		2, 1, 3,
		// right
		2, 3, 5,
		5, 3, 7,
		// back
		5, 7, 4,
		4, 7, 6,
		// left
		4, 6, 0,
		0, 6, 1,
		// top
		4, 0, 5,
		5, 0, 2,
		// bottom
		1, 6, 3,
		3, 6, 7
	};

	debugMesh = new Mesh(vertices, indices, shader);
}

void Grid3D::draw()
{
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			for (int z = 0; z < length; z++)
			{
				glm::mat4 model = glm::translate(glm::mat4(1.f), cells[x][y][z]->pos);
				shader.setMat4("model", model);
				debugMesh->draw();
			}
		}
	}
}

Cell* Grid3D::getCellFromPosition(glm::vec3 pos)
{
	float sizeX = width * cellSize;
	float sizeY = height * cellSize;
	float sizeZ = length * cellSize;
	int x = ((pos.x + (sizeX / 2) / cellSize));
	int y = ((pos.y + (sizeY / 2) / cellSize));
	int z = ((pos.z + (sizeZ / 2) / cellSize));

	if (x < 0 || x >= width || y < 0 || y >= height || z < 0 || z >= length) return nullptr;
	
	return cells[x][y][z];
}

std::vector<Cell*> Grid3D::getCellNeighbours(Cell* cell)
{
	std::vector<Cell*> neighbours;
	int iters = 0;
	for (int x = -1; x <= 1; x++)
	{
		for (int y = -1; y <= 1; y++)
		{
			for (int z = -1; z <= 1; z++)
			{
				iters++;
				// current cell
				if (x == 0 && y == 0 && z == 0) { 
					//std::cout << "not checking self" << std::endl;
					continue; }

				// out of range
				if (cell->x + x < 0 || cell->x + x >= width) { 
					//std::cout << cell->x + x << "not in range " << width << std::endl;
					continue; 
				}
				if (cell->y + y < 0 || cell->y + y >= height) {
					//std::cout << cell->y + y << "not in range " << height << std::endl;
					continue;
				}
				if (cell->z + z < 0 || cell->z + z >= length) {
					//std::cout << cell->z + z << " z not in range " << length << std::endl;
					continue;
				}

				neighbours.push_back(cells[cell->x + x][cell->y + y][cell->z + z]);
			}
		}
	}
	return neighbours;
}

std::vector<Particle*> Grid3D::getNeighbouringPaticlesInRadius(Particle* particle, float radius)
{
	std::vector<Particle*> parts;
	Cell* current = getCellFromPosition(particle->getPosition());
	if (current == nullptr) return parts;
	std::vector<Cell*> cells = getCellNeighbours(current);

	for (int i = 0; i < current->particles.size(); i++)
	{
		if (current->particles[i]->getId() != particle->getId() &&
			glm::distance(current->particles[i]->getPosition(), particle->getPosition()) < radius)
			parts.push_back(current->particles[i]);
	}

	for (int i = 0; i < cells.size(); i++)
	{
		for (int j = 0; j < cells[i]->particles.size(); j++)
		{
			if(glm::distance(particle->getPosition(), cells[i]->particles[j]->getPosition()) <= radius)
				parts.push_back(cells[i]->particles[j]);
		}
	}
	return parts;
}

Cell::Cell(int x, int y, int z, glm::vec3 pos, float size, bool debug, Shader& shader)
	:x(x), y(y), z(z), pos(pos), size(size), shader(shader)
{
}

