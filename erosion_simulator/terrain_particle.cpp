#include "terrain_particle.h"

TerrainParticle::TerrainParticle(glm::vec3 position, float radius, int coordX, int coordY) :
	Particle(position, radius),
	_coordX(coordX),
	_coordY(coordY)
{
}
