#include "sph_particle.h"

SphParticle::SphParticle(glm::vec3 position, float radius) : Particle(position, radius) {
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

