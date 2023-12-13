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
	return 0.001 * glm::tanh(1 - _sedimentCarry);
}
float SphParticle::takeSediment(float amount) {

	if (_sedimentCarry + amount > sedimentSaturation)
		amount = sedimentSaturation - _sedimentCarry;
	else if (_sedimentCarry + amount < 0)
		amount = 0 - _sedimentCarry;
	_sedimentCarry += amount;
	mass += amount;
	return amount;
}