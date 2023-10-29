#pragma once
#include "mesh/mesh.h"
#include "glm/glm.hpp"

class Particle
{
public:
	Particle(glm::vec3 position);
	void setPosition(glm::vec3 position);	
	glm::vec3 getPosition();
private:
	glm::vec3 position;
};

