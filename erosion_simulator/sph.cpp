#include <algorithm>
#include <numeric>
#include <glm/gtx/norm.hpp>
#include <neighborTable.h>
#include <sph.h>
#include <iostream>
#define PI 3.14159265359f


SPHSettings::SPHSettings(
	float mass, float restDensity, float gasConst, float viscosity, float h,
	float g, float tension)
	: mass(mass)
	, restDensity(restDensity)
	, gasConstant(gasConst)
	, viscosity(viscosity)
	, h(h)
	, g(g)
	, tension(tension)
{
	poly6 = 315.0f / (64.0f * PI * pow(h, 9));
	spikyGrad = -45.0f / (PI * pow(h, 6));
	spikyLap = 45.0f / (PI * pow(h, 6));
	h2 = h * h;
	selfDens = mass * poly6 * pow(h, 6);
	massPoly6Product = mass * poly6;
	//sphereScale = glm::scale(glm::vec3(h / 2.f));
}


//Calculate the particles' density and pressure based on the neighbors. Can be computed in parallel
void parallelDensityAndPressures(
	SphParticle* particle, std::vector<SphParticle*> neighbours,
	const SPHSettings& settings)
{
	float massPoly6Product = settings.mass * settings.poly6;

	float pDensity = 0;
	for (size_t i = 0; i < neighbours.size(); i++) {
		
		float dist2 = glm::length2(neighbours[i]->getPosition() - particle->getPosition());
		if (dist2 < settings.h2) {
			pDensity += massPoly6Product * glm::pow(settings.h2 - dist2, 3); //kernel function
		}
	}

	// Include self density (as itself isn't included in neighbour)
	particle->setDensity(pDensity + settings.selfDens);

	// Calculate pressure
	float pPressure = settings.gasConstant * (particle->getDensity() - settings.restDensity);
	particle->setPressure(pPressure);
	
}

void parallelForces(
	SphParticle* particle, std::vector<SphParticle*> neighbours,
	const SPHSettings& settings)
{
	particle->setForce(glm::vec3(0));
	for (size_t i = 0; i < neighbours.size(); i++) {
				
		float dist2 = glm::length2(particle->getPosition() - neighbours[i]->getPosition());
		if (dist2 < settings.h2) {
			//unit direction and length
			float dist = sqrt(dist2);
			glm::vec3 dir = glm::normalize(neighbours[i]->getPosition() - particle->getPosition());

			// if (dist < 0.1) dir *= -1.f;

			//apply pressure force
			glm::vec3 pressureForce = -dir * settings.mass * (particle->getPressure() + neighbours[i]->getPressure()) / (2 * neighbours[i]->getDensity()) * settings.spikyGrad;
			pressureForce *= std::pow(settings.h - dist, 2);
			particle->setForce(particle->getForce() + pressureForce);

			//apply viscosity force
			glm::vec3 velocityDif = neighbours[i]->getVelocity() - particle->getVelocity();
			glm::vec3 viscoForce = settings.viscosity * settings.mass * (velocityDif / neighbours[i]->getDensity()) * settings.spikyLap * (settings.h - dist);
			particle->setForce(particle->getForce() + viscoForce);

			//apply sping repulsion
			//if (dist < 1) 
			//{
			//	// std::cout << (10000.f * dir).x << std::endl;
			//	particle->setForce(particle->getForce() + (10000.f * dir));
			//}
		}
	}
}


void parallelUpdateParticlePositions(
	std::vector<SphParticle*> particles, const SPHSettings& settings, const float& deltaTime)
{

	float boxWidth = 8.f;
	float elasticity = 0.5f;

	for (size_t i = 0; i < particles.size(); i++) {
		SphParticle* p = particles[i];

		//calculate acceleration and velocity
		glm::vec3 acceleration = p->getForce() / p->getDensity() + glm::vec3(0, settings.g, 0);
		p->setVelocity(p->getVelocity() + acceleration * deltaTime);

		// Update position
		p->setPosition(p->getPosition() + p->getVelocity() * deltaTime);

		//The boundry & Terrain collison handling need to be implemented 
	
	}
}



