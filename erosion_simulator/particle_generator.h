#pragma once
#include "particle.h"
#include "mesh/mesh.h"
#include "shader/shader.h"
#include <vector>

class ParticleGenerator
{
public:
	ParticleGenerator(Shader& shader, Mesh* mesh, int mapWidth, int mapLength, int numPerSquare);
	void draw();
	void updateParticles(float deltaTime, float time);
private:

	// instanced array
	uint32_t buffer;

	std::vector<Particle> particles;
	std::vector<glm::mat4> particleModels;
	Shader shader;
	Mesh* particleMesh;
	
};

