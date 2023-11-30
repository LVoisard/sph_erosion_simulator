#include "terrain_particle.h"

int TerrainParticle::next_id = 0;

TerrainParticle::TerrainParticle(glm::vec3 position, float radius, int coordX, int coordY) :
	Particle(position, radius),
	_coordX(coordX),
	_coordY(coordY)
{
	id = TerrainParticle::next_id++;
}
