#include "particle_generator.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <exception>

ParticleGenerator::ParticleGenerator(Shader& shader, Mesh* mesh, HeightMap* map, float cellSize, int numPerSquare)
	:shader(shader), particleMesh(mesh), _heightmap(map)
{
	float mapWidth = _heightmap->getWidth();
	float mapLength = _heightmap->getLength();
	float maxHeight = _heightmap->getMaxHeight();
	float height = _heightmap->getHeight();

	// <= on loop condidtion since we need one more row and column than the map dimensions to complete the grid
	for (int x = 0; x <= mapWidth; x++) {
		for (int y = 0; y <= mapLength; y++) {
			glm::vec3 position(x, map->samplePoint(x, y), y);
			TerrainParticle* p = new TerrainParticle(position, x, y);
			terrainParticles.push_back(p);

			// assign models, but I'm not sure where we can make them get drawn
			particleModelsTerrain.push_back(glm::translate(glm::mat4(1), p->getPosition()));
		}
	}

	std::cout << " height " << mapWidth << std::endl;
	for (int x = 0; x < mapWidth / cellSize * numPerSquare; x++)
	{
		for (int y = 0; y < (4 * numPerSquare); y++)
		{
			for (int z = 0; z < mapLength / cellSize * numPerSquare; z++)
			{
				//((maxHeight - y) + (float) (y * cellSize / 2 / (numPerSquare)) - (cellSize / 2 / (numPerSquare)))
				glm::vec3 pos(
					(float)x * cellSize / (numPerSquare) - (float)mapWidth / 2 + (cellSize / 2 / (numPerSquare)) ,
					(float)((maxHeight - y * cellSize / numPerSquare)) - (cellSize / 2 / (numPerSquare)),
					(float)z * cellSize / (numPerSquare) - (float)mapLength / 2 + (cellSize / 2 / (numPerSquare)));
				SphParticle* p = new SphParticle(pos);
				sphParticles.push_back(p);
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

	std::vector<Particle*> particleVector(sphParticles.begin(), sphParticles.end()); // turn vector<SphParticle*> into vector<Particle*> (implicit casting isn't possible)
	grid = Grid3D(mapWidth, mapLength, height, cellSize, particleVector, shader);

	std::cout << "Generated " << sphParticles.size() << " particles" << std::endl;
}

void ParticleGenerator::drawParticles()
{	
	particleMesh->drawInstanced(sphParticles.size());
}

void ParticleGenerator::drawGridDebug()
{
	grid.draw();
}

static int iter = 0;
static float timePast = 0;
void ParticleGenerator::updateParticles(float deltaTime, float time)
{
	for (int i = 0; i < sphParticles.size(); i++)
	{
		// before updating particle position
		Cell* previousCell = grid.getCellFromPosition(sphParticles[i]->getPosition());
		
		// update particle
		glm::vec3 pos = sphParticles[i]->getPosition();
		sphParticles[i]->setPosition(pos + glm::vec3(0, sin(pos.x + pos.z + time) * 0.25 * deltaTime - deltaTime, 0));
		
		// search for neighbours
		std::vector<Particle*> cells = grid.getNeighbouringPaticlesInRadius(sphParticles[i]);

		//after updating particle position
		Cell* currentCell = grid.getCellFromPosition(sphParticles[i]->getPosition());
		if (previousCell != currentCell)
		{
			if (previousCell != nullptr)
				previousCell->removeParticle(sphParticles[i]);
			if(currentCell != nullptr)
				currentCell->addParticle(sphParticles[i]);
		}

		particleModels[i] = glm::translate(glm::mat4(1.0), sphParticles[i]->getPosition());
	}

	int mapWidth = _heightmap->getWidth();
	int mapHeight = _heightmap->getLength();
	int mapLength = _heightmap->getHeight();

	int x = iter % mapWidth;
	int z = (iter / mapWidth) % mapLength;
	int y = ((iter / mapWidth) / mapLength) % mapHeight;
	int particleID = iter % sphParticles.size();


	if (timePast > (float)60 / sphParticles.size() ) {
		for (int i = 0; i < sphParticles.size(); i++)
		{
			particleDebugs[i].isNearestNeighbour = false;
			particleDebugs[i].isNearestNeighbourTarget = false;
		}

		Cell* cell = grid.getCellFromPosition(sphParticles[particleID]->getPosition());
		std::vector<Particle*> parts = grid.getNeighbouringPaticlesInRadius(sphParticles[particleID]);
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
