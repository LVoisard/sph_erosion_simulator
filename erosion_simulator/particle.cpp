#include "particle.h"


Particle::Particle(glm::vec3 position, int id)
    :position(position), id(id)
{
}

void Particle::setPosition(glm::vec3 position)
{
    this->position = position;
}

glm::vec3 Particle::getPosition()
{
    return position;
}
