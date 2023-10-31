#pragma once
#include "particle.h"
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

class ParticleGenerator
{
public:
	ParticleGenerator(Shader& shader, Mesh* mesh, int mapWidth, int mapLength, int numPerSquare);
	void drawParticles();
	void drawGridDebug();
	void updateParticles(float deltaTime, float time);
private:

	int width;
	int height;
	int length;

	// instanced array
	uint32_t buffer;
	uint32_t annBuffer;

	std::vector<Particle*> particles;
	std::vector<glm::mat4> particleModels;
	std::vector<ParticleDebug> particleDebugs;
	Shader shader;
	Mesh* particleMesh;
	Grid3D grid;
	
};

