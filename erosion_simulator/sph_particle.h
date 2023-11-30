#pragma once
#include "glm/glm.hpp"
#include "particle.h"

class SphParticle : public Particle {
public:
	SphParticle(glm::vec3 position, float radius);
	void update(float deltaTime, float time) override;
	void setVelocity(glm::vec3 newVelocity);	
	void setDensity(float newDensity);

	glm::vec3 getVelocity() const { return velocity; }
	float getDensity() const { return density; }


	// to be filled in
private:
	glm::vec3 velocity = glm::vec3(0);
	float density = 1;
	static int next_id; // Ids are assigned on creation, with no overlap. This keeps track of the id that will be assigned to the next created particle.
};