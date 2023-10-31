#include "particle_generator.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <exception>

ParticleGenerator::ParticleGenerator(Shader& shader, Mesh* mesh, int mapWidth, int mapLength, int maxHeight, int height, float cellSize, int numPerSquare)
	:shader(shader), particleMesh(mesh), width(mapWidth), height(height), length(mapLength)
{
	int particleId = 0;
	std::cout << " height " << width << std::endl;
	for (int x = 0; x < width / cellSize * numPerSquare; x++)
	{
		for (int y = 0; y < (4 * numPerSquare); y++)
		{
			for (int z = 0; z < length / cellSize * numPerSquare; z++)
			{
				//((maxHeight - y) + (float) (y * cellSize / 2 / (numPerSquare)) - (cellSize / 2 / (numPerSquare)))
				glm::vec3 pos(
					(float)x * cellSize / (numPerSquare) - (float)mapWidth / 2 + (cellSize / 2 / (numPerSquare)) ,
					(float)((maxHeight - y * cellSize / numPerSquare)) - (cellSize / 2 / (numPerSquare)),
					(float)z * cellSize / (numPerSquare) - (float)mapLength / 2 + (cellSize / 2 / (numPerSquare)));
				Particle* p = new Particle(pos, particleId++);
				particles.push_back(p);
				particleModels.push_back(glm::translate(glm::mat4(1), p->getPosition()));
				particleDebugs.push_back(ParticleDebug());
			}
		}
	}

	unsigned int VAO = particleMesh->getVAO();

	glBindVertexArray(VAO);

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, particleModels.size() * sizeof(glm::mat4), particleModels.data(), GL_STATIC_DRAW);
		
	// set attribute pointers for matrix (4 times vec4)
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);
	glEnableVertexAttribArray(6);

	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(1 * sizeof(glm::vec4)));
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// debug information 

	glGenBuffers(1, &annBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, annBuffer);
	glBufferData(GL_ARRAY_BUFFER, particleDebugs.size() * sizeof(ParticleDebug), particleDebugs.data(), GL_STATIC_DRAW);

	
	glEnableVertexAttribArray(7);
	glEnableVertexAttribArray(8);
	glVertexAttribPointer(7, 1, GL_INT, GL_FALSE, sizeof(ParticleDebug), (void*)0);
	glVertexAttribPointer(8, 1, GL_INT, GL_FALSE, sizeof(ParticleDebug), (void*)(sizeof(int)));
	glVertexAttribDivisor(7, 1);
	glVertexAttribDivisor(8, 1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	grid = Grid3D(mapWidth, mapLength, height, cellSize, particles, shader);

	std::cout << "Generated " << particles.size() << " particles" << std::endl;
}

void ParticleGenerator::drawParticles()
{	
	particleMesh->drawInstanced(particles.size());
}

void ParticleGenerator::drawGridDebug()
{
	grid.draw();
}

static int iter = 0;
static float timePast = 0;
void ParticleGenerator::updateParticles(float deltaTime, float time)
{
	for (int i = 0; i < particles.size(); i++)
	{
		// before updating particle position
		Cell* previousCell = grid.getCellFromPosition(particles[i]->getPosition());
		
		// update particle
		glm::vec3 pos = particles[i]->getPosition();
		particles[i]->setPosition(pos + glm::vec3(0, sin(pos.x + pos.z + time) * 0.25 * deltaTime - deltaTime, 0));
		
		// search for neighbours
		std::vector<Particle*> cells = grid.getNeighbouringPaticlesInRadius(particles[i]);

		//after updating particle position
		Cell* currentCell = grid.getCellFromPosition(particles[i]->getPosition());
		if (previousCell != currentCell)
		{
			if (previousCell != nullptr)
				previousCell->removeParticle(particles[i]);
			if(currentCell != nullptr)
				currentCell->addParticle(particles[i]);
		}

		particleModels[i] = glm::translate(glm::mat4(1.0), particles[i]->getPosition());
	}
	int x = iter % width;
	int z = (iter / width) % length;
	int y = ((iter / width) / length) % height;
	int particleID = iter % particles.size();


	if (timePast > (float)60 / particles.size() ) {
		for (int i = 0; i < particles.size(); i++)
		{
			particleDebugs[i].isNearestNeighbour = false;
			particleDebugs[i].isNearestNeighbourTarget = false;
		}

		Cell* cell = grid.getCellFromPosition(particles[particleID]->getPosition());
		std::vector<Particle*> parts = grid.getNeighbouringPaticlesInRadius(particles[particleID]);
		particleDebugs[particleID].isNearestNeighbourTarget = true;
		for (int i = 0; i < parts.size(); i++)
		{
			particleDebugs[parts[i]->getId()].isNearestNeighbour = true;
		}
		timePast = 0;
		iter++;
	}
	timePast += deltaTime;
	
	/*printf("Nearest Neighbour node to (%f, %f, %f) (ID: %d) is (%f, %f, %f) (ID: %d)\n", 
		particles[0]->getPosition().x, particles[0]->getPosition().y, particles[0]->getPosition().z,
		particles[0]->getId(),
		node->particle->getPosition().x, node->particle->getPosition().y, node->particle->getPosition().z,
		node->particle->getId());*/

	// update the models buffer
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	void* data = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	memcpy(data, particleModels.data(), sizeof(particleModels[0]) * particleModels.size());
	glUnmapBuffer(GL_ARRAY_BUFFER);


	// update the debug buffer
	glBindBuffer(GL_ARRAY_BUFFER, annBuffer);
	void* debugData = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	memcpy(debugData, particleDebugs.data(), sizeof(particleDebugs[0]) * particleDebugs.size());
	glUnmapBuffer(GL_ARRAY_BUFFER);
	
	//particleMesh->update();
}
