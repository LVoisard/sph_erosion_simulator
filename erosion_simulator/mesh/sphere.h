#pragma once
#include "glm/glm.hpp"
#include "shader/shader.h"
#include "mesh.h"

class Sphere : public Mesh
{
public:
	Sphere(Sphere* sphere);
	Sphere(glm::vec3 position, float radius, Shader shader);
	virtual void init() override;
private:
	glm::vec3 center;
	int radius;
};

