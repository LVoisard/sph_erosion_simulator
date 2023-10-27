#include "particle.h"


Particle::Particle(Sphere* sphere)
    :velocity(glm::vec3(0)), position(glm::vec3(0))
{
    this->mesh = new Sphere(sphere);
    this->mesh->init();
}

void Particle::setPosition(glm::vec3 position)
{
    for (int i = 0; i < mesh->vertices.size(); i++)
    {
        mesh->vertices[i].pos += position - this->position;
    }
    this->position = position;
    mesh->update();
}

glm::vec3 Particle::getPosition()
{
    return position;
}

void Particle::draw()
{
    mesh->draw();
}
