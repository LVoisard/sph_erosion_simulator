#pragma once
#include "mesh/mesh.h"
#include "glm/glm.hpp"

class Particle
{
public:
	Particle(glm::vec3 position, int id);
	void setPosition(glm::vec3 position);	
	glm::vec3 getPosition();
	int getId() { return id; }
private:
	int id;
	glm::vec3 position;
};

