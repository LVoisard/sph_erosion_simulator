#pragma once
#include "mesh/mesh.h"
#include "glm/glm.hpp"


class Particle
{
public:
    Particle(glm::vec3 position, float radius);

    // Virtual destructor in case this class is intended to be a base class
    virtual ~Particle() {}

    virtual void update(float deltaTime, float time);

    // Setter functions
    void setPosition(glm::vec3 newPosition);
    void setRadius(float newRadius);
    void setId(int newId);

    // Getter functions
    glm::vec3 getPosition() const { return position; }
    float getRadius() const { return radius; }
    int getId() const { return id; }

protected:
    glm::vec3 position;
    float radius;
    int id; // largely used for debug purposes
   
};

