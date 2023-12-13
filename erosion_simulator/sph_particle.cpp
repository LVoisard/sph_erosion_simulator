#include "sph_particle.h"
#include <iostream>

#define PI 3.14159265359f

SphParticle::SphParticle(glm::vec3 position, float radius) : _radius(radius), Particle(position, radius) {
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
void SphParticle::setSedimentDensity(float newDensity)
{
	sedimentDensity = newDensity;
}
void SphParticle::setSediment(float sediment)
{
	_sedimentCarry = sediment;
	// mass = baseMass + _sedimentCarry;
}
float SphParticle::getSedimentTake() const {
	return 0.001 * glm::tanh(1 - _sedimentCarry);
}
float SphParticle::takeSediment(float amount) {

	if (_sedimentCarry + amount > sedimentSaturation) {
		amount = sedimentSaturation - _sedimentCarry;
	}
	else if (_sedimentCarry + amount < 0)
		amount = 0 - _sedimentCarry;

	_sedimentCarry += amount;
	//mass += amount;
	return amount;
}

float SphParticle::getSedimentVolume()
{
	return _sedimentCarry / (4.0f / 3.0f * PI * powf(radius, 3));
}

float SphParticle::getMaxSedimentVolume() const
{
	return sedimentSaturation / (4.0f / 3.0f * PI * powf(radius, 3));
}

