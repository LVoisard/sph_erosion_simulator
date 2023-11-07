#include "particle_generator.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <exception>

ParticleGenerator::ParticleGenerator(Shader& shader, Mesh* sphMesh, Mesh* boundaryMesh, HeightMap* map, float terrainSpacing, float cellSize, float particleRadius, int numPerSquare)
	:shader(shader), particleMesh(sphMesh), terrainParticlesMesh(boundaryMesh), _heightmap(map)
{
	float mapWidth = _heightmap->getWidth();
	float mapLength = _heightmap->getLength();
	float maxHeight = _heightmap->getMaxHeight();
	float height = _heightmap->getHeight();	

	std::cout << " height " << (mapWidth - 1) << std::endl;
	float sphOffset = (cellSize / 2 / (numPerSquare));
	for (int x = 0; x < (mapWidth - 1) / cellSize * numPerSquare; x++)
	{
		for (int y = 0; y < (4 * numPerSquare); y++)
		{
			for (int z = 0; z < (mapLength - 1) / cellSize * numPerSquare; z++)
			{
				//((maxHeight - y) + (float) (y * cellSize / 2 / (numPerSquare)) - (cellSize / 2 / (numPerSquare)))
				glm::vec3 pos(
					(float)x * cellSize / (numPerSquare) - (float)(mapWidth - 1) / 2 + sphOffset,
					(float)((maxHeight - y * cellSize / numPerSquare)) - sphOffset,
					(float)z * cellSize / (numPerSquare) - (float)(mapLength - 1) / 2 + sphOffset);
				SphParticle* sphPart = new SphParticle(pos * terrainSpacing, particleRadius);
				sphParticles.push_back(sphPart);
				particleModels.push_back(glm::translate(glm::mat4(1), sphPart->getPosition()));
				sphParticleDebugs.push_back(SPHParticleDebug());
			}
		}
	}

	for (int x = 0; x < mapWidth; x++) {
		for (int y = 0; y < mapLength; y++) {
			glm::vec3 position = map->getPositionAtIndex(x, y);
			TerrainParticle* terrainPart = new TerrainParticle(position, particleRadius, x, y);
			terrainParticles.push_back(terrainPart);
			boundaryParticleDebugs.push_back(BoundaryParticleDebug());

			// assign models, but I'm not sure where we can make them get drawn
			particleModelsTerrain.push_back(glm::translate(glm::mat4(1), terrainPart->getPosition()));
		}
	}

	glBindVertexArray(particleMesh->getVAO());

	glGenBuffers(1, &sphBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, sphBuffer);
	glBufferData(GL_ARRAY_BUFFER, particleModels.size() * sizeof(glm::mat4), particleModels.data(), GL_DYNAMIC_DRAW);
		
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

	glGenBuffers(1, &sphDebugBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, sphDebugBuffer);
	glBufferData(GL_ARRAY_BUFFER, sphParticleDebugs.size() * sizeof(SPHParticleDebug), sphParticleDebugs.data(), GL_DYNAMIC_DRAW);
	
	glEnableVertexAttribArray(7);
	glEnableVertexAttribArray(8);
	glVertexAttribPointer(7, 1, GL_INT, GL_FALSE, sizeof(SPHParticleDebug), (void*)0);
	glVertexAttribPointer(8, 1, GL_INT, GL_FALSE, sizeof(SPHParticleDebug), (void*)(sizeof(int)));
	glVertexAttribDivisor(7, 1);
	glVertexAttribDivisor(8, 1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	glBindVertexArray(terrainParticlesMesh->getVAO());
	// terrain particles

	glGenBuffers(1, &terrainParticlesBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, terrainParticlesBuffer);
	glBufferData(GL_ARRAY_BUFFER, particleModelsTerrain.size() * sizeof(glm::mat4), particleModelsTerrain.data(), GL_DYNAMIC_DRAW);

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

	glGenBuffers(1, &terrainParticlesDebugBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, terrainParticlesDebugBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(BoundaryParticleDebug) * boundaryParticleDebugs.size(), boundaryParticleDebugs.data(), GL_DYNAMIC_DRAW);
	
	glEnableVertexAttribArray(7);

	glVertexAttribPointer(7, 1, GL_INT, GL_FALSE, sizeof(int), (void*)0);

	glVertexAttribDivisor(7, 1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glBindVertexArray(0);

	std::vector<Particle*> particleVector(sphParticles.begin(), sphParticles.end()); // turn vector<SphParticle*> into vector<Particle*> (implicit casting isn't possible)
	particleVector.insert(particleVector.end(), terrainParticles.begin(), terrainParticles.end());
	grid = Grid3D(mapWidth - 1, mapLength - 1, height,terrainSpacing,  cellSize, particleVector, shader);

	std::cout << "Generated " << sphParticles.size() << " sph particles" << std::endl;
	std::cout << "Generated " << terrainParticles.size() << " terrain particles" << std::endl;
	std::cout << "Generated " << sphParticles.size() + terrainParticles.size() << " total particles" << std::endl;
}

void ParticleGenerator::drawParticles()
{	
	particleMesh->drawInstanced(particleModels.size());
}

void ParticleGenerator::drawTerrainParticles()
{
	terrainParticlesMesh->drawInstanced(particleModelsTerrain.size());
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
		sphParticles[i]->update(deltaTime, time);

		// if the particle is below the terrain, bring it back.
		// UNCOMMENT BELOW
		glm::vec3 pos = sphParticles[i]->getPosition();
		float terrainHeightAtPosition = _heightmap->sampleHeightAtPosition(pos.x, pos.z);
		if (pos.y < terrainHeightAtPosition) {
			sphParticles[i]->setPosition(glm::vec3(pos.x, terrainHeightAtPosition, pos.z));
			// should perform some operation on the velocity and pressure here as well.
		}
		particleModels[i] = glm::translate(glm::mat4(1.0), sphParticles[i]->getPosition());
		
		// search for neighbours
		std::vector<Particle*> parts = grid.getNeighbouringPaticlesInRadius(sphParticles[i]);

		//after updating particle position
		Cell* currentCell = grid.getCellFromPosition(sphParticles[i]->getPosition());
		if (previousCell != currentCell)
		{
			if (previousCell != nullptr)
				previousCell->removeParticle(sphParticles[i]);
			if(currentCell != nullptr)
				currentCell->addParticle(sphParticles[i]);
		}

	}

	int mapWidth = _heightmap->getWidth();
	int mapHeight = _heightmap->getLength();
	int mapLength = _heightmap->getHeight();

	int x = iter % mapWidth;
	int z = (iter / mapWidth) % mapLength;
	int y = ((iter / mapWidth) / mapLength) % mapHeight;
	int particleID = iter % sphParticles.size();

	// debug all particles in one minute (of fps allows it)

	if (timePast > (float)240 / sphParticles.size() ) {
		for (int i = 0; i < sphParticleDebugs.size(); i++)
		{
			sphParticleDebugs[i].isNearestNeighbour = false;
			sphParticleDebugs[i].isNearestNeighbourTarget = false;
		}
		for (int i = 0; i < boundaryParticleDebugs.size(); i++)
		{
			boundaryParticleDebugs[i].isNearestNeighbour = false;
		}

		Cell* cell = grid.getCellFromPosition(sphParticles[particleID]->getPosition());
		std::vector<Particle*> parts = grid.getNeighbouringPaticlesInRadius(sphParticles[particleID]);
		sphParticleDebugs[particleID].isNearestNeighbourTarget = true;
		for (int i = 0; i < parts.size(); i++)
		{
			SphParticle* sph = dynamic_cast<SphParticle*>(parts[i]);
			TerrainParticle* bound = dynamic_cast<TerrainParticle*>(parts[i]);
			if (sph != 0)
				sphParticleDebugs[parts[i]->getId()].isNearestNeighbour = true;
			else if (bound != 0) {
				boundaryParticleDebugs[parts[i]->getId() - sphParticles.size()].isNearestNeighbour = true;
			}
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
	glBindBuffer(GL_ARRAY_BUFFER, sphBuffer);
	void* data = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	memcpy(data, particleModels.data(), sizeof(particleModels[0]) * particleModels.size());
	glUnmapBuffer(GL_ARRAY_BUFFER);

	// update the models buffer
	glBindBuffer(GL_ARRAY_BUFFER, terrainParticlesBuffer);
	void* terrainData = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	memcpy(terrainData, particleModelsTerrain.data(), sizeof(particleModelsTerrain[0]) * particleModelsTerrain.size());
	glUnmapBuffer(GL_ARRAY_BUFFER);

	// update the debug buffer
	glBindBuffer(GL_ARRAY_BUFFER, sphDebugBuffer);
	void* debugData = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	memcpy(debugData, sphParticleDebugs.data(), sizeof(sphParticleDebugs[0]) * sphParticleDebugs.size());
	glUnmapBuffer(GL_ARRAY_BUFFER);

	glBindBuffer(GL_ARRAY_BUFFER, terrainParticlesDebugBuffer);
	void* boundaryDebugData = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	memcpy(boundaryDebugData, boundaryParticleDebugs.data(), sizeof(boundaryParticleDebugs[0]) * boundaryParticleDebugs.size());
	glUnmapBuffer(GL_ARRAY_BUFFER);
	
	//particleMesh->update();
}
