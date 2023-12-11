#include "sph_particle.h"

SphParticle::SphParticle(glm::vec3 position, float radius) : _radius(radius), _sedimentCarry(0), Particle(position, radius) {
}

void SphParticle::update(float deltaTime, float time)
{
	// position += glm::vec3(0, sin(position.x + position.z + time) * 0.25 * deltaTime - deltaTime, 0);
}
void SphParticle::setVelocity(glm::vec3 newVelocity) {
	velocity = newVelocity;
};
void SphParticle::setDensity(float newDensity) {
	density = newDensity;
}
float SphParticle::getSedimentTake() const {
	return 0.05 * glm::tanh(1 - _sedimentCarry);
}
void SphParticle::takeSediment(float amount) {
	_sedimentCarry += amount;
}