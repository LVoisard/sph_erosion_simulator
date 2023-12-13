#include <algorithm>
#include <numeric>
#include <glm/gtx/norm.hpp>
#include <sph.h>
#include <iostream>
#define PI 3.14159265359f


SPHSettings::SPHSettings(
	float mass, float restDensity, float pressureMultiplier, float nearPressureMultiplier, float viscosity, float h,
	float g, float sedimentSaturation, float timeStep)
	: mass(mass)
	, restDensity(restDensity)
	, pressureMultiplier(pressureMultiplier)
	, surfaceTensionMultiplier(nearPressureMultiplier)
	, viscosity(viscosity)
	, h(h)
	, g(g)
	, sedimentSaturation(sedimentSaturation)
	, timeStep(timeStep)
{
	h2 = h * h;
}

// 3.5 
// Smoothing kernel
// https://matthias-research.github.io/pages/publications/sca03.pdf

// what good about this kernel is that h is already to the power of two, and so is x
// sqrt is an expensive computation
float kernelFuncSmooth(float h2, float x2)
{
	if (x2 > h2) return 0;

	float integral = (64 * PI * powf(sqrt(h2), 9)) / 315.0f;
	float fx = powf(h2 - x2, 3);
	return fx / integral;
}

// this kernel is specific to pressure, because the smooth kernels have a vanishing gradient at the center
// this allows for partciles to be repulsed by eachother.
// https://matthias-research.github.io/pages/publications/sca03.pdf
float kernelFuncSpiky3(float h, float x)
{
	if (x > h) return 0;

	float integral = (PI * powf(h, 6)) / 15.0f;
	float fx = powf(h - x, 3);
	return fx / integral;
}

// https://github.com/SebLague/Fluid-Sim
float kernelFuncSpiky2(float h, float x)
{
	if (x > h) return 0;

	float integral = (2 * PI * pow(h, 5)) / 15.0f;
	float fx = powf(h - x, 2);
	return fx / integral;

}

// https://matthias-research.github.io/pages/publications/sca03.pdf
float kernelFuncViscosity(float h, float dist)
{
	if (dist > h) return 0;
	float integral = (2 * PI * powf(h, 3)) / 15.0f;
	float fx = -(powf(dist, 3) / (2 * powf(h, 3))) + (powf(dist, 2) / (powf(h, 2))) + (h / (2 * dist)) - 1;
	return fx / integral;
}


void calculateDensity(SphParticle* particle, std::vector<SphParticle*> neighbours,
	const SPHSettings& settings)
{
	float density = 0;
	for (int i = 0; i < neighbours.size(); i++) {

		float dist2 = glm::length2((neighbours[i]->getPosition() + neighbours[i]->getVelocity() * settings.timeStep) - (particle->getPosition() + particle->getVelocity() * settings.timeStep));
		if (dist2 > settings.h2) continue;
		float dist = sqrt(dist2);
		density += neighbours[i]->mass * kernelFuncSpiky3(settings.h, dist);
	}

	// add particle self density
	density += particle->mass * kernelFuncSpiky3(settings.h, 0);

	particle->setDensity(density);
}

void calculateSedimentDensity(SphParticle* particle, std::vector<SphParticle*> neighbours,
	const SPHSettings& settings)
{
	float density = 0;
	for (int i = 0; i < neighbours.size(); i++) {

		float dist2 = glm::length2((neighbours[i]->getPosition() + neighbours[i]->getVelocity() * settings.timeStep) - (particle->getPosition() + particle->getVelocity() * settings.timeStep));
		if (dist2 > settings.h2) continue;
		float dist = sqrt(dist2);
		density += neighbours[i]->mass * kernelFuncSpiky3(settings.h, dist) * neighbours[i]->getSedimentVolume() / neighbours[i]->getMaxSedimentVolume();
	}

	// add particle self density
	density += particle->mass * kernelFuncSpiky3(settings.h, 0) * particle->getSedimentVolume() / particle->getMaxSedimentVolume();

	particle->setSedimentDensity(density);
}

float getPressureFromDensity(float density, const SPHSettings& settings)
{
	return (density - settings.restDensity) * settings.pressureMultiplier;
}

void calculatePressureForce(SphParticle* particle, std::vector<SphParticle*> neighbours, const SPHSettings& settings)
{
	glm::vec3 pressureForce(0);

	for (int i = 0; i < neighbours.size(); i++)
	{
		glm::vec3 ab = (neighbours[i]->getPosition() + neighbours[i]->getVelocity() * settings.timeStep) - (particle->getPosition() + particle->getVelocity() * settings.timeStep);
		
		if (glm::length2(ab) > settings.h2) continue;
		float dist = glm::length(ab);
		glm::vec3 dir = ab / std::max(0.001f, dist);
		float presure = (getPressureFromDensity(particle->getDensity(), settings) + getPressureFromDensity(neighbours[i]->getDensity(), settings)) / 2;
		pressureForce += -dir * kernelFuncSpiky3(settings.h, dist) * neighbours[i]->mass * presure / neighbours[i]->getDensity();
	}
	particle->setVelocity(particle->getVelocity() + pressureForce / particle->getDensity() * settings.timeStep);
}

void calculateViscosity(SphParticle* particle, std::vector<SphParticle*> neighbours, const SPHSettings& settings)
{
	glm::vec3 viscosityForce(0);
	for (int i = 0; i < neighbours.size(); i++)
	{
		glm::vec3 ab = (neighbours[i]->getPosition() + neighbours[i]->getVelocity() * settings.timeStep) - (particle->getPosition() + particle->getVelocity() * settings.timeStep);
		if (glm::length2(ab) > settings.h2) continue;

		float dist = glm::length(ab);
		viscosityForce += (neighbours[i]->getVelocity() - particle->getVelocity()) / neighbours[i]->getDensity() * kernelFuncViscosity(settings.h, dist) * neighbours[i]->mass;
	}

	particle->setVelocity(particle->getVelocity() + viscosityForce * settings.viscosity * settings.timeStep);
}

void calculateSufaceTension(SphParticle* particle, std::vector<SphParticle*> neighbours, const SPHSettings& settings)
{
	glm::vec3 surfaceTensionForce(0);
	for (int i = 0; i < neighbours.size(); i++)
	{
		glm::vec3 ab = (particle->getPosition() + particle->getVelocity() * settings.timeStep) - (neighbours[i]->getPosition() + neighbours[i]->getVelocity() * settings.timeStep);
		if (glm::length2(ab) > settings.h2) continue;

		float dist = glm::length(ab);
		surfaceTensionForce += ab * kernelFuncSpiky2(settings.h, dist) * neighbours[i]->mass;
	}

	particle->setVelocity(particle->getVelocity() + -settings.surfaceTensionMultiplier / particle->mass * surfaceTensionForce * settings.timeStep);
}

