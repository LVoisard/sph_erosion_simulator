#include "terrain_particle.h"

TerrainParticle::TerrainParticle(glm::vec3 position, int coordX, int coordY) :
	Particle(position),
	_coordX(coordX),
	_coordY(coordY)
{}
