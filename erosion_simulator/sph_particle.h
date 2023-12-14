#pragma once
#include "glm/glm.hpp"
#include "particle.h"

class SphParticle : public Particle {
public:
	SphParticle(glm::vec3 position, float radius);
	void update(float deltaTime, float time) override;
	void setVelocity(glm::vec3 newVelocity);	
	void setDensity(float newDensity);
	void setSedimentDensity(float newDensity);
	void setSediment(float sediment);

	glm::vec3 getVelocity() const { return velocity; }
	float getDensity() const { return density; }
	float getSedimentDensity() const { return sedimentDensity; }
	float getSediment() const { return _sedimentCarry; }
	float takeSediment(float);
	float getSedimentVolume();
	float getMaxSedimentVolume() const;

	glm::vec3 sedimentSettlingVelocity = glm::vec3(0);

	float mass = 1;
	const float sedimentSaturation = 1; // likely not realistic, but better for showcasing erosion
	// to be filled in
private:
	glm::vec3 velocity = glm::vec3(0);
	float baseMass = 1;
	float density = 1;
	float sedimentDensity = 0;
	float _radius;
	float _sedimentCarry = 0.0;
};