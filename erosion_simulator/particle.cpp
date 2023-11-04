#include "particle.h"

// initialize the particle ids at 0
int Particle::next_id = 0;

Particle::Particle(glm::vec3 position)
    :position(position), id(Particle::next_id++)
{
}

glm::vec3 Particle::getPosition()
{
    return position;
}
void Particle::setPosition(glm::vec3 position)
{
    this->position = position;
}