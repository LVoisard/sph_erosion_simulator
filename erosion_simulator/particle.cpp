#include "particle.h"

// initialize the particle ids at 0
int Particle::next_id = 0;

Particle::Particle(glm::vec3 position, float radius)
    :position(position), radius(radius), id(Particle::next_id++)
{
}

void Particle::setPosition(glm::vec3 position)
{
    this->position = position;
}