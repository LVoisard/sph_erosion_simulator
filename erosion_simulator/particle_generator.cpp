#include "particle_generator.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include <exception>
#include <unordered_map>

ParticleGenerator::ParticleGenerator(Shader& shader, Mesh* sphMesh, Mesh* boundaryMesh, HeightMap* map, TerrainMesh* terrain, float terrainSpacing, float cellSize, float particleRadius, int numPerSquare, SPHSettings* settings)
	:shader(shader), particleMesh(sphMesh), terrainParticlesMesh(boundaryMesh), _heightmap(map), terrain(terrain), settings(settings)
{
	float mapWidth = _heightmap->getWidth();
	float mapLength = _heightmap->getLength();
	float maxHeight = _heightmap->getMaxHeight();
	float height = _heightmap->getHeight();	

	std::cout << " height " << (mapWidth - 1) << std::endl;
	float sphOffset = (cellSize / 1.9 / (numPerSquare));
	for (int x = 0; x < (mapWidth - 1) / cellSize * numPerSquare / 4; x++)
	{
		for (int y = 0; y < 4 * (numPerSquare); y++)
		{
			for (int z = 0; z < (mapLength - 1) / cellSize * numPerSquare / 4; z++)
			{
				//((maxHeight - y) + (float) (y * cellSize / 2 / (numPerSquare)) - (cellSize / 2 / (numPerSquare)))
				glm::vec3 pos(
					(float)x * sphOffset - (float)(mapWidth - 1) / 8 + sphOffset,
					(float)(((maxHeight - 1)/2 - y * sphOffset)) - sphOffset,
					(float)z * sphOffset - (float)(mapLength - 1) / 8 + sphOffset);
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
	glEnableVertexAttribArray(9);
	glVertexAttribPointer(7, 1, GL_INT, GL_FALSE, sizeof(SPHParticleDebug), (void*)0);
	glVertexAttribPointer(8, 1, GL_INT, GL_FALSE, sizeof(SPHParticleDebug), (void*)(sizeof(int)));
	glVertexAttribPointer(9, 1, GL_FLOAT, GL_FALSE, sizeof(SPHParticleDebug), (void*)(sizeof(int) * 2));
	glVertexAttribDivisor(7, 1);
	glVertexAttribDivisor(8, 1);
	glVertexAttribDivisor(9, 1);

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

	std::vector<SphParticle*> sphParticleVector(sphParticles.begin(), sphParticles.end()); // turn vector<SphParticle*> into vector<Particle*> (implicit casting isn't possible)
	std::vector<TerrainParticle*> terrainParticleVector(terrainParticles.begin(), terrainParticles.end());
	grid = Grid3D(mapWidth - 1, mapLength - 1, height, terrainSpacing,  cellSize, sphParticleVector, terrainParticleVector, shader);

	//settings.restDensity = ((mapWidth - 1) * (mapLength - 1) * height) / (settings.mass * sphParticles.size()) * cellSize;

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
	deltaTime = 0.0045;
	std::unordered_map <SphParticle*,std::vector<SphParticle*>> particleNeigbours;

	for (int i = 0; i < sphParticles.size(); i++)
	{
		// before updating particle position
		
		// update particle
		std::vector<SphParticle*> parts = grid.getNeighbouringSPHPaticlesInRadius(sphParticles[i]);
		std::pair<SphParticle*, std::vector<SphParticle*>> pair(sphParticles[i], parts);
		particleNeigbours.insert(pair);
		calculateDensity(sphParticles[i], particleNeigbours.at(sphParticles[i]), *settings);
	}
	for (int i = 0; i < sphParticles.size(); i++)
	{
		// before updating particle position
		calculatePressureForce(sphParticles[i], particleNeigbours.at(sphParticles[i]), *settings);

		calculateSufaceTension(sphParticles[i], particleNeigbours.at(sphParticles[i]), *settings);
		calculateViscosity(sphParticles[i], particleNeigbours.at(sphParticles[i]), *settings);
	}

	for (int i = 0; i < sphParticles.size(); i++)
	{
		Cell* previousCell = grid.getCellFromPosition(sphParticles[i]->getPosition());
		glm::vec3 acceleration = glm::vec3(0, settings->g, 0);

		sphParticles[i]->setVelocity(sphParticles[i]->getVelocity() + (acceleration) * deltaTime);
		sphParticles[i]->setPosition(sphParticles[i]->getPosition() + sphParticles[i]->getVelocity() * deltaTime);


		glm::vec3 pos = sphParticles[i]->getPosition();
		glm::vec3 vel = sphParticles[i]->getVelocity();
		float rad = sphParticles[i]->getRadius();
		// std::cout << pos.x << " " << pos.y << " " << pos.z << std::endl;
		
		if (pos.x - rad < _heightmap->getMinX() || pos.x + rad >= _heightmap->getMaxX() - 1) {
			sphParticles[i]->setPosition(glm::vec3(pos.x - rad < _heightmap->getMinX() ? _heightmap->getMinX() + rad : _heightmap->getMaxX() - 1 - rad, pos.y, pos.z));
			sphParticles[i]->setVelocity(glm::vec3(-vel.x * 0.05f, vel.y, vel.z));
			//std::cout << "outside X bounds" << std::endl;
		}

		pos = sphParticles[i]->getPosition();
		vel = sphParticles[i]->getVelocity();
		if (pos.z - rad < _heightmap->getMinZ() || pos.z + rad >= _heightmap->getMaxZ() - 1) {
			//std::cout << "outside Z bounds" << std::endl;
			sphParticles[i]->setPosition(glm::vec3(pos.x, pos.y, pos.z - rad < _heightmap->getMinZ() ? _heightmap->getMinZ() + rad : _heightmap->getMaxZ() - 1 - rad));
			sphParticles[i]->setVelocity(glm::vec3(vel.x, vel.y, -vel.z * 0.05f));
		}


		pos = sphParticles[i]->getPosition();
		vel = sphParticles[i]->getVelocity();
		// if the particle is below the terrain, bring it back.
		// UNCOMMENT BELOW
		float terrainHeightAtPosition = _heightmap->sampleHeightAtPosition(pos.x, pos.z);
		if (sphParticles[i]->getPosition().y - rad <= terrainHeightAtPosition) {

			glm::vec3 normal = _heightmap->sampleNormalAtPosition(pos.x, pos.z);
			float yStrength = std::clamp(1 - glm::dot(normal, glm::normalize(sphParticles[i]->getVelocity())), 0.05f, 0.95f);
			sphParticles[i]->setPosition(glm::vec3(pos.x, terrainHeightAtPosition + rad, pos.z));
			glm::vec3 newVel = glm::reflect(sphParticles[i]->getVelocity(), normal);
			sphParticles[i]->setVelocity(glm::vec3(newVel.x * 0.95f, newVel.y * yStrength, newVel.z * 0.95f));
			// sphParticles[i]->setVelocity(glm::vec3(vel.x, -vel.y * 0.2, vel.z));
			// should perform some operation on the velocity and pressure here as well.
		}
		else if (sphParticles[i]->getPosition().y + rad > _heightmap->getMaxHeight() - 1)
		{
			sphParticles[i]->setPosition(glm::vec3(pos.x, _heightmap->getMaxHeight() - 1 - rad - 0.001, pos.z));
			sphParticles[i]->setVelocity(glm::vec3(vel.x * 0.5f, -vel.y * 0.05, vel.z * 0.5));
		}

		// Update position
		particleModels[i] = glm::translate(glm::mat4(1.0), sphParticles[i]->getPosition());
		// search for neighbours

		//after updating particle position
		Cell* currentCell = grid.getCellFromPosition(sphParticles[i]->getPosition());
		if (previousCell != currentCell)
		{
			if (previousCell != nullptr)
				previousCell->removeSphParticle(sphParticles[i]);
			if (currentCell != nullptr)
				currentCell->addSphParticle(sphParticles[i]);
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
	std::cout << glm::length(sphParticles[0]->getVelocity()) << std::endl;

	for (int i = 0; i < sphParticleDebugs.size(); i++)
	{
		sphParticleDebugs[i].isNearestNeighbour = false;
		sphParticleDebugs[i].isNearestNeighbourTarget = false;
		sphParticleDebugs[i].linearVelocity = glm::length2(sphParticles[i]->getVelocity());
	}
	for (int i = 0; i < boundaryParticleDebugs.size(); i++)
	{
		boundaryParticleDebugs[i].isNearestNeighbour = false;
	}

	Cell* cell = grid.getCellFromPosition(sphParticles[particleID]->getPosition());
	std::vector<SphParticle*> parts = grid.getNeighbouringSPHPaticlesInRadius(sphParticles[particleID]);
	std::cout << parts.size() << "neighbours" << std::endl;

	sphParticleDebugs[particleID].isNearestNeighbourTarget = true;
	for (int i = 0; i < parts.size(); i++)
	{
		sphParticleDebugs[parts[i]->getId()].isNearestNeighbour = true;
	}
	if (timePast > (float)60 / sphParticles.size() ) {
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
