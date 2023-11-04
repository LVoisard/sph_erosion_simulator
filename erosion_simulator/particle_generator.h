#pragma once
#include "sph_particle.h"
#include "terrain_particle.h"
#include "mesh/mesh.h"
#include "shader/shader.h"
#include <vector>
#include "grid_3d.h"

struct ParticleDebug {
	int isNearestNeighbourTarget;
	int isNearestNeighbour;

	ParticleDebug() {
		isNearestNeighbourTarget = 0;
		isNearestNeighbour = 0;
	}
};

// Represents the system responsible for all particles of the erosion model.
class ParticleGenerator
{
public:
	ParticleGenerator(Shader& shader, Mesh* mesh, HeightMap* map, float cellSize, int numPerSquare);
	void drawParticles();
	void drawGridDebug();
	void updateParticles(float deltaTime, float time);
private:

	HeightMap* _heightmap;

	// instanced array
	uint32_t buffer = 0;
	uint32_t annBuffer = 0;

	std::vector<SphParticle*> sphParticles;
	std::vector<TerrainParticle*> terrainParticles;

	std::vector<glm::mat4> particleModels;
	std::vector<glm::mat4> particleModelsTerrain;
	std::vector<ParticleDebug> particleDebugs;
	Shader shader;
	Mesh* particleMesh;
	Grid3D grid;
	
};

