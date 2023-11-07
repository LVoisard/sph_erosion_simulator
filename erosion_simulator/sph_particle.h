#pragma once
#include "glm/glm.hpp"
#include "particle.h"

class SphParticle : public Particle {
public:
	SphParticle(glm::vec3 position, float radius);
	void update(float deltaTime, float time) override;

	// to be filled in
};