#include "particle.h"

Particle::Particle(glm::vec3 position, float radius)
    :position(position), radius(radius)
{
}

void Particle::update(float deltaTime, float time)
{
}

void Particle::setPosition(glm::vec3 position)
{
    this->position = position;
}