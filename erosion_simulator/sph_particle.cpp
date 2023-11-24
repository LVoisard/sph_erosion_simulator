#include "sph_particle.h"

SphParticle::SphParticle(glm::vec3 position, float radius) : Particle(position, radius) {
	/*float vx = rand() % 1000 / 100.0;
	float vy = rand() % 1000 / 100.0;
	float vz = rand() % 1000 / 100.0;

	float px = (rand() % 10 - 4) * rand() % 10 / 10.f;
	float py = (rand() % 10 - 4) * rand() % 10 / 10.f;
	float pz = (rand() % 10 - 4) * rand() % 10 / 10.f;*/


	//velocity = glm::vec3(vx, vy, vz);
	//this->position = glm::vec3(px, py, pz);
}

void SphParticle::update(float deltaTime, float time)
{
	position += glm::vec3(0, sin(position.x + position.z + time) * 0.25 * deltaTime - deltaTime, 0);
}
void SphParticle::setVelocity(glm::vec3 newVelocity) {
	velocity = newVelocity;
};

void SphParticle::setAcceleration(glm::vec3 newAcceleration) {
	acceleration = newAcceleration;
};

void SphParticle::setForce(glm::vec3 newForce) {
	force = newForce;
};

void SphParticle::setPressure(float newPressure) {
	pressure = newPressure;
};

void SphParticle::setHash(uint16_t newHash) {
	hash = newHash;
};

void SphParticle::setDensity(float newDensity) {
	density = newDensity;
};
