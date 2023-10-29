#include "particle_generator.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

ParticleGenerator::ParticleGenerator(Shader& shader, Mesh* mesh, int mapWidth, int mapLength, int numPerSquare)
	:shader(shader), particleMesh(mesh)
{
	for (int x = 0; x < mapWidth * numPerSquare; x++)
	{
		for (int y = 0; y < 1; y++)
		{
			for (int z = 0; z < mapLength * numPerSquare; z++)
			{
				glm::vec3 pos((float)x / numPerSquare - (float)mapWidth / 2, (float)y, (float)z / numPerSquare - (float)mapLength / 2);
				Particle p(pos);				
				particles.push_back(p);
				particleModels.push_back(glm::translate(glm::mat4(1), p.getPosition()));
			}
		}
	}

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, particleModels.size() * sizeof(glm::mat4), &particleModels[0], GL_STATIC_DRAW);

	for (int i = 0; i < particleModels.size(); i++)
	{
		unsigned int VAO = particleMesh->getVAO();

		glBindVertexArray(VAO);
		// set attribute pointers for matrix (4 times vec4)
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	std::cout << "Generated " << particles.size() << " particles" << std::endl;
}

void ParticleGenerator::draw()
{	
	particleMesh->drawInstanced(particles.size());
}

void ParticleGenerator::updateParticles(float deltaTime, float time)
{
	for (int i = 0; i < particles.size(); i++)
	{
		glm::vec3 pos = particles[i].getPosition();
		particles[i].setPosition(pos + glm::vec3(0, sin(pos.x + pos.z + time) * 0.25 * deltaTime, 0));	
		particleModels[i] = glm::translate(glm::mat4(1.0), particles[i].getPosition());
	}

	// update the models buffer
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	void* data = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	memcpy(data, particleModels.data(), sizeof(particleModels[0]) * particleModels.size());
	glUnmapBuffer(GL_ARRAY_BUFFER);
	
	//particleMesh->update();
}
