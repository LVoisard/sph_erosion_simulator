#include "grid_3d.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include <iostream>

Grid3D::Grid3D()
{

}

Grid3D::Grid3D(int width, int length, int height, float terrainSpacing, float cellSize, std::vector<SphParticle*> sphParticles, std::vector<TerrainParticle*> terrainParticles, Shader& shader)
	:width(ceil(width / cellSize)), length(ceil(length / cellSize)), height(ceil(height / cellSize)), cellSize(cellSize * terrainSpacing), particleSearchRadius(cellSize * terrainSpacing), shader(shader)
{
	cells = new Cell***[this->width];
	for (int x = 0; x < this->width; x++)
	{
		cells[x] = new Cell**[this->height];
		for (int y = 0; y < this->height; y++)
		{
			cells[x][y] = new Cell*[this->length];
			for (int z = 0; z < this->length; z++)
			{
				cells[x][y][z] = new Cell(x,y,z, 
					glm::vec3(
						x - this->width / 2, 
						y - this->height / 2, 
						z - this->length / 2) * this->cellSize, this->cellSize, false, shader);
			}
		}
	}

	for (int i = 0; i < sphParticles.size(); i++)
	{
		Cell* cell = getCellFromPosition(sphParticles[i]->getPosition());
		if (cell != nullptr)
			cell->addSphParticle(sphParticles[i]);
	}

	for (int i = 0; i < terrainParticles.size(); i++)
	{
		Cell* cell = getCellFromPosition(terrainParticles[i]->getPosition());
		if (cell != nullptr)
			cell->addTerrainParticle(terrainParticles[i]);
	}

	std::vector<Vertex> vertices = {

			Vertex(glm::vec3(0, 1, 0), glm::vec3(0.0f,0.0f,0.0f), glm::vec2(0.0f,0.0f)), // 0
			Vertex(glm::vec3(0, 0, 0), glm::vec3(0.0f,0.0f,0.0f), glm::vec2(0.0f,0.0f)), // 1
			Vertex(glm::vec3(1, 1, 0), glm::vec3(0.0f,0.0f,0.0f), glm::vec2(0.0f,0.0f)), // 2
			Vertex(glm::vec3(1, 0, 0), glm::vec3(0.0f,0.0f,0.0f), glm::vec2(0.0f,0.0f)), // 3

			Vertex(glm::vec3(0, 1, 1), glm::vec3(0.0f,0.0f,0.0f), glm::vec2(0.0f,0.0f)), // 4
			Vertex(glm::vec3(1, 1, 1), glm::vec3(0.0f,0.0f,0.0f), glm::vec2(0.0f,0.0f)), // 5
			Vertex(glm::vec3(0, 0, 1), glm::vec3(0.0f,0.0f,0.0f), glm::vec2(0.0f,0.0f)), // 6
			Vertex(glm::vec3(1, 0, 1), glm::vec3(0.0f,0.0f,0.0f), glm::vec2(0.0f,0.0f)), // 7
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
				model = glm::scale(model, glm::vec3(cellSize));
				shader.setMat4("model", model);
				debugMesh->draw();
			}
		}
	}
}

Cell* Grid3D::getCellFromPosition(glm::vec3 pos)
{
	float sizeX = width / cellSize;
	float sizeY = height / cellSize;
	float sizeZ = length / cellSize;
	int x = ((pos.x / cellSize + (float)(width / 2)));
	int y = ((pos.y / cellSize + (float)(height / 2)));
	int z = ((pos.z / cellSize + (float)(length / 2)));

	if (x < 0 || x >= width || y < 0 || y >= height || z < 0 || z >= length) { 
		// std::cout << "outside : " << x << ", " << y << ", " << z << std::endl;
		return nullptr; }
	
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

std::vector<SphParticle*> Grid3D::getNeighbouringSPHPaticlesInRadius(Particle* particle)
{
	std::vector<SphParticle*> parts;
	Cell* current = getCellFromPosition(particle->getPosition());
	if (current == nullptr) return parts;
	std::vector<Cell*> cells = getCellNeighbours(current);

	float searchRadius2 = particleSearchRadius * particleSearchRadius + particle->getRadius() * particle->getRadius();

	for (int i = 0; i < current->sphParticles.size(); i++)
	{
		if (current->sphParticles[i]->getId() != particle->getId() &&
			glm::distance2(current->sphParticles[i]->getPosition(), particle->getPosition()) <= searchRadius2)
			parts.push_back(current->sphParticles[i]);
	}

	for (int i = 0; i < cells.size(); i++)
	{
		for (int j = 0; j < cells[i]->sphParticles.size(); j++)
		{
			if(glm::distance2(particle->getPosition(), cells[i]->sphParticles[j]->getPosition()) <= searchRadius2)
				parts.push_back(cells[i]->sphParticles[j]);
		}
	}
	return parts;
}

Cell::Cell(int x, int y, int z, glm::vec3 pos, float size, bool debug, Shader& shader)
	:x(x), y(y), z(z), pos(pos), size(size), shader(shader)
{
}

void Cell::removeSphParticle(SphParticle* p)
{
	for (int i = 0; i < sphParticles.size(); i++)
	{
		if (sphParticles[i] == p) {
			sphParticles.erase(sphParticles.begin() + i);
			break;
		}
	}
}

void Cell::addSphParticle(SphParticle* p)
{
	sphParticles.push_back(p);
}

void Cell::removeTerrainParticle(TerrainParticle* p)
{
	for (int i = 0; i < terrainParticles.size(); i++)
	{
		if (terrainParticles[i] == p) {
			terrainParticles.erase(terrainParticles.begin() + i);
			break;
		}
	}
}

void Cell::addTerrainParticle(TerrainParticle* p)
{
	terrainParticles.push_back(p);
}

