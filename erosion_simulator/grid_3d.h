#pragma once
#include <vector>
#include "shader/shader.h"
#include "particle.h"
#include "sph_particle.h"
#include "terrain_particle.h"

class Cell
{
public:
	Cell(int x, int y, int z, glm::vec3 pos, float size, bool debug, Shader& shader);
	void draw();
	int x, y, z;
	glm::vec3 pos;
	float size;
	void removeSphParticle(SphParticle* p);
	void addSphParticle(SphParticle* p);
	void removeTerrainParticle(TerrainParticle* p);
	void addTerrainParticle(TerrainParticle* p);
	std::vector<SphParticle*> sphParticles;
	std::vector<TerrainParticle*> terrainParticles;
private:

	Shader shader;
	Mesh* debugMesh;
};

class Grid3D
{
public:
	Grid3D();
	Grid3D(int width, int length, int height, float terrainSpacing, float cellSize, std::vector<SphParticle*> sphParticles, std::vector<TerrainParticle*> terrainParticles, Shader & shader);
	void draw();
	Cell* getCellFromPosition(glm::vec3 pos);
	std::vector<Cell*> getCellNeighbours(Cell* cell);
	std::vector<SphParticle*> getNeighbouringSPHPaticlesInRadius(Particle* particle);
	Cell**** cells;
private:
	Shader shader;
	float particleSearchRadius;
	int width, length, height;
	float cellSize;
	Mesh* debugMesh;
};

