#pragma once
#include "mesh/mesh.h"
#include "glm/glm.hpp"

class Particle
{
public:
	Particle(glm::vec3 position);

	glm::vec3 getPosition() const { return position; };
	void setPosition(glm::vec3 newPosition) { position = newPosition; }
	
	int getId() const { return id; }

protected:
	glm::vec3 position;

private:
	int id;	// largely used for debug purposes
	static int next_id; // Ids are assigned on creation, with no overlap. This keeps track of id that will be assigned to the next created particle.
};

