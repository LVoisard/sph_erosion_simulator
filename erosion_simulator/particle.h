#pragma once
#include "mesh/sphere.h"
#include "glm/glm.hpp"

class Particle
{
public:
	Particle(Sphere* sphere);
	void update(float deltaTime);
	void setPosition(glm::vec3 position);
	glm::vec3 getPosition();
	void draw();
private:

	Sphere* mesh;
	glm::vec3 position;
	glm::vec3 velocity;
};

