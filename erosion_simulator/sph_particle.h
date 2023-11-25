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
	void setNearDensity(float newNearDensity);
	void setViscosity(glm::vec3 viscosity);
	void setHash(uint16_t newHash);

	glm::vec3 getVelocity() const { return velocity; }
	glm::vec3 getAcceleration() const { return acceleration; }
	glm::vec3 getForce() const { return force; }
	glm::vec3 getViscosity() const { return viscosity; }
	float getPressure() const { return pressure; }
	float getDensity() const { return density; }
	float getNearDensity() const { return density; }
	
	uint16_t getHash() const { return hash; }


	// to be filled in
private:
	glm::vec3 velocity = glm::vec3(0), acceleration = glm::vec3(0), force = glm::vec3(0), viscosity = glm::vec3(0);
	float pressure = 1;

	float density = 1;
	float nearDensity = 1;
	uint16_t hash;
};