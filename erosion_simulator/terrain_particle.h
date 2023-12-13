#pragma once
#include "glm/glm.hpp"
#include "particle.h"

class TerrainParticle : public Particle {
public:
	TerrainParticle(glm::vec3 positon, float radius, int coordX, int coordY);

	int _coordX;
	int _coordY;
private:
	// there's likely a better data structure for this (is there an int-only vector2?)
};