#include "particle.h"


Particle::Particle(Sphere* sphere)
    :mesh(sphere), velocity(glm::vec3(0)), position(glm::vec3(0))
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

void Particle::draw()
{
    mesh->draw();
}
