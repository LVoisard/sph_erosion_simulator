#pragma once
#include "mesh/mesh.h"
#include "glm/glm.hpp"

class Particle
{
public:
	Particle(glm::vec3 position, float radius);

	void setPosition(glm::vec3 position);	
	glm::vec3 getPosition() { return position; }

	float getRadius() { return radius; }
	
	int getId() { return id; }

private:
	glm::vec3 position;
	float radius;

	int id;	// largely used for debug purposes
	static int next_id; // Ids are assigned on creation, with no overlap. This keeps track of id that will be assigned to the next created particle.
};

