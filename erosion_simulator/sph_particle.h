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
	float getSediment() const { return _sedimentCarry; }
	float getSedimentTake() const;
	float takeSediment(float);

	glm::vec3 sedimentSettlingVelocity = glm::vec3(0);

	float mass = 1;
	// to be filled in
private:
	glm::vec3 velocity = glm::vec3(0);
	float baseMass = 1;
	float density = 1;
	float sedimentSaturation = 0.1;
	float _radius;
	float _sedimentCarry;
};