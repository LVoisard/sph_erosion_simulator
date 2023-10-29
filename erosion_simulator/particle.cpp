#include "particle.h"


Particle::Particle(glm::vec3 position)
    :position(position)
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
