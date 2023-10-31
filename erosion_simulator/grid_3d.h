#pragma once
#include <vector>
#include "shader/shader.h"
#include "particle.h"
class Cell
{
public:
	Cell(int x, int y, int z, glm::vec3 pos, float size, bool debug, Shader& shader);
	void draw();
	int x, y, z;
	glm::vec3 pos;
	float size;
	void removeParticle(Particle* p);
	void addParticle(Particle* p);
	std::vector<Particle*> particles;
private:

	std::vector<Cell*> neighbours;
	Shader shader;
	Mesh* debugMesh;
};

class Grid3D
{
public:
	Grid3D();
	Grid3D(int width, int length, int height, float cellSize, std::vector<Particle*> particles, Shader & shader);
	void draw();
	Cell* getCellFromPosition(glm::vec3 pos);
	std::vector<Cell*> getCellNeighbours(Cell* cell);
	std::vector<Particle*> getNeighbouringPaticlesInRadius(Particle* particle);
	Cell**** cells;
private:
	Shader shader;
	float particleSearchRadius;
	int width, length, height;
	float cellSize;
	Mesh* debugMesh;
};

