#pragma once
#include "sph.h"
#include "sph_particle.h"
#include "terrain_particle.h"
#include "mesh/mesh.h"
#include "shader/shader.h"
#include <vector>
#include "grid_3d.h"
#include "mesh/terrain_mesh.h"

struct SPHParticleDebug {
	int isNearestNeighbourTarget;
	int isNearestNeighbour;

	SPHParticleDebug() {
		isNearestNeighbourTarget = 0;
		isNearestNeighbour = 0;
	}
};

struct BoundaryParticleDebug {
	int isNearestNeighbour;
	BoundaryParticleDebug() {
		isNearestNeighbour = 0;
	}
};

// Represents the system responsible for all particles of the erosion model.
class ParticleGenerator
{
public:
	ParticleGenerator(Shader& shader, Mesh* sphMesh, Mesh* boundaryMesh, HeightMap* map, TerrainMesh* terrain, float terrainSpacing, float cellSize, float particleRadius, int numPerSquare, SPHSettings* settings);
	void drawParticles();
	void drawTerrainParticles();
	void drawGridDebug();
	void updateParticles(float deltaTime, float time);

private:

	HeightMap* _heightmap;
	TerrainMesh* terrain;

	SPHSettings* settings;

	// instanced array
	uint32_t sphBuffer = 0;
	uint32_t sphDebugBuffer = 0;
	uint32_t terrainParticlesBuffer = 0;
	uint32_t terrainParticlesDebugBuffer = 0;

	std::vector<SphParticle*> sphParticles;
	std::vector<TerrainParticle*> terrainParticles;

	std::vector<glm::mat4> particleModels;
	std::vector<glm::mat4> particleModelsTerrain;
	std::vector<SPHParticleDebug> sphParticleDebugs;
	std::vector<BoundaryParticleDebug> boundaryParticleDebugs;

	Shader shader;
	Mesh* particleMesh;
	Mesh* terrainParticlesMesh;
	Grid3D grid;
	
};

