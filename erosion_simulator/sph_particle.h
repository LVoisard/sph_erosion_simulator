#pragma once
#include "glm/glm.hpp"
#include "particle.h"

class SphParticle : public Particle {
public:
	SphParticle(glm::vec3 position, float radius);
	void update(float deltaTime, float time) override;
	void setVelocity(glm::vec3 newVelocity);
	void setAcceleration(glm::vec3 newAcceleration);
	void setForce(glm::vec3 newForce);
	void setPressure(float newPressure);
	void setDensity(float newDensity);
	void setHash(uint16_t newHash);

	glm::vec3 getVelocity() const { return velocity; }
	glm::vec3 getAcceleration() const { return acceleration; }
	glm::vec3 getForce() const { return force; }
	float getPressure() const { return pressure; }
	float getDensity() const { return density; }
	uint16_t getHash() const { return hash; }


	// to be filled in
private:
	glm::vec3 velocity, acceleration, force;
	float pressure;
	float density;
	uint16_t hash;
};