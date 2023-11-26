#include <algorithm>
#include <numeric>
#include <glm/gtx/norm.hpp>
#include <sph.h>
#include <iostream>
#define PI 3.14159265359f


SPHSettings::SPHSettings(
	float mass, float restDensity, float pressureMultiplier, float nearPressureMultiplier, float viscosity, float h,
	float g, float timeStep)
	: mass(mass)
	, restDensity(restDensity)
	, pressureMultiplier(pressureMultiplier)
	, nearPressureMultiplier(nearPressureMultiplier)
	, viscosity(viscosity)
	, h(h)
	, g(g)
	, timeStep(timeStep)
{
	h2 = h * h;
	//sphereScale = glm::scale(glm::vec3(h / 2.f));
}

// 3.5 
// Smoothing kernel
// https://matthias-research.github.io/pages/publications/sca03.pdf
float kernelFuncSmooth(float h, float x)
{
	if (x > h) return 0;

	float integral = 315 / (64 * PI * powf(h, 9));
	float fx = powf((powf(h, 2) - powf(x, 2)), 3);
	return fx * integral;
}

// this kernel is specific to pressure, because the smooth kernels have a vanishing gradient at the center
// this allows for partciles to be repulsed by eachother.
// https://matthias-research.github.io/pages/publications/sca03.pdf
float kernelFuncSpiky3(float h, float x)
{
	if (x > h) return 0;

	float integral = 15.f / (PI * powf(h, 6));
	float fx = powf(h - x, 3);
	return fx * integral;
}

float kernelFuncSpiky3Derivative(float h, float x)
{
	if (x > h) return 0;

	float integral = 15.f / (PI * powf(h, 6));
	float fxdx = -3 * powf((h - x), 2);
	return fxdx * integral;
}

// https://github.com/SebLague/Fluid-Sim
float kernelFuncSpiky2(float h, float x)
{
	if (x > h) return 0;

	float integral = 15 / (2 * PI * pow(h, 5));
	float fx = powf(h - x, 2);
	return fx * integral;

}

// https://github.com/SebLague/Fluid-Sim
float kernelFuncSpiky2Derivative(float h, float x)
{
	if (x > h) return 0;

	float integral = 15 / (2 * pow(h, 5) * PI);
	float fxdx = -2 * (h - x);
	return fxdx * integral;
}


// https://matthias-research.github.io/pages/publications/sca03.pdf
float kernelFuncViscosity(float h, float dist)
{
	if (dist > h) return 0;
	float integral = 15 / (2 * PI * powf(h, 3));
	float fx = -(powf(dist, 3) / (2 * powf(h, 3))) + (powf(dist, 2) / (powf(h, 2))) + (h / (2 * dist)) - 1;
	return fx * integral;
}


void calculateDensity(SphParticle* particle, std::vector<SphParticle*> neighbours,
	const SPHSettings& settings)
{
	float density = 0;
	float nearDensity = 0;
	for (int i = 0; i < neighbours.size(); i++) {

		float dist2 = glm::length2((neighbours[i]->getPosition() + neighbours[i]->getVelocity() * settings.timeStep) - (particle->getPosition() + particle->getVelocity() * settings.timeStep));
		if (dist2 > settings.h2) continue;
		float dist = sqrt(dist2);
		density += settings.mass * kernelFuncSpiky2(settings.h, dist);
		nearDensity += settings.mass * kernelFuncSpiky3(settings.h, dist);
		// std::cout << "joe" << SpikyKernelPow2(settings.h, dist) << std::endl;
	}

	// add particle self density
	density += settings.mass * kernelFuncSpiky2(settings.h, 0);
	nearDensity += settings.mass * kernelFuncSpiky3(settings.h, 0);

	particle->setDensity(density);
	particle->setNearDensity(nearDensity);
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
		float nearPresure = (particle->getNearDensity() + neighbours[i]->getNearDensity()) * settings.nearPressureMultiplier / 2;
		pressureForce += dir * kernelFuncSpiky2Derivative(settings.h, dist) * settings.mass * presure / neighbours[i]->getDensity();
		pressureForce += dir * kernelFuncSpiky3Derivative(settings.h, dist) * settings.mass * nearPresure / neighbours[i]->getNearDensity();
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
		viscosityForce += (neighbours[i]->getVelocity() - particle->getVelocity()) / neighbours[i]->getDensity() * kernelFuncSmooth(settings.h, dist) * settings.mass;
	}

	particle->setVelocity(particle->getVelocity() + viscosityForce * settings.viscosity * settings.timeStep);
}

