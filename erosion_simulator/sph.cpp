#include <algorithm>
#include <numeric>
#include <glm/gtx/norm.hpp>
#include <sph.h>
#include <iostream>
#define PI 3.14159265359f


SPHSettings::SPHSettings(
	float mass, float restDensity, float gasConst, float viscosity, float h,
	float g, float timeStep)
	: mass(mass)
	, restDensity(restDensity)
	, pressureMultiplier(gasConst)
	, viscosity(viscosity)
	, h(h)
	, g(g)
	, timeStep(timeStep)
{
	h2 = h * h;
	//sphereScale = glm::scale(glm::vec3(h / 2.f));
}

// 3.5 Smoothing kernel
// https://matthias-research.github.io/pages/publications/sca03.pdf
float kernelFunc(float h, float dist)
{
	if (dist > h) {
		return 0;
	}

	float integral = 315 / (64 * PI * powf(h, 9));
	return powf((powf(h, 2) - powf(dist, 2)), 3) * integral;
}

// this kernel is specific to pressure, because the smooth kernels have a vanishing gradient at the center
// this allows for partciles to be repulsed by eachother.
// https://matthias-research.github.io/pages/publications/sca03.pdf
float kernelFuncPressure(float h, float dist)
{
	if (dist > h) return 0;
	float integral = 15.f / (2 * PI * powf(h, 5));
	return powf(h - dist, 2) * integral;
}

// https://matthias-research.github.io/pages/publications/sca03.pdf
float kernelFuncPressureDerivative(float h, float dist)
{
	if (dist > h)return 0;
	float integral = 45.f / (powf(h, 6) * PI);
	float v = h - dist;
	return -v * v * integral;
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

		float dist = glm::length((neighbours[i]->getPosition() + neighbours[i]->getVelocity() * 0.0045f) - (particle->getPosition() + particle->getVelocity() * 0.0045f));
		if (dist > settings.h + particle->getRadius()) continue;
		density += settings.mass * kernelFuncPressure(settings.h + particle->getRadius(), dist); //kernel function
		nearDensity += settings.mass * kernelFunc(settings.h + particle->getRadius(), dist); //kernel function
	}

	// add particle self density
	density += settings.mass * kernelFuncPressure(settings.h + particle->getRadius(), 0);
	nearDensity += settings.mass * kernelFunc(settings.h + particle->getRadius(), 0); //kernel function

	particle->setDensity(density);
	particle->setNearDensity(nearDensity);
}

void calculatePressure(SphParticle* particle, std::vector<SphParticle*> neighbours,
	const SPHSettings& settings)
{
	float pressure = 0;
	for (int i = 0; i < neighbours.size(); i++)
	{
		pressure += (neighbours[i]->getDensity() - settings.restDensity) * settings.pressureMultiplier;
	}
	particle->setPressure(pressure);
}

void calculatePressureForce(SphParticle* particle, std::vector<SphParticle*> neighbours, const SPHSettings& settings)
{
	glm::vec3 pressureForce(0);

	for (int i = 0; i < neighbours.size(); i++)
	{
		glm::vec3 ab = (neighbours[i]->getPosition() + neighbours[i]->getVelocity() * settings.timeStep) - (particle->getPosition() + particle ->getVelocity() * settings.timeStep);
		float dist = glm::length(ab);
		if (dist > settings.h + particle->getRadius()) continue;
		glm::vec3 dir = ab / std::max(0.001f, dist);
		
		pressureForce += dir * kernelFuncPressureDerivative(settings.h + particle->getRadius(), dist) * settings.mass / neighbours[i]->getDensity();
		// pressureForce += dir * kernelFuncPressure(settings.h + particle->getRadius(), dist) * settings.mass * presure / neighbours[i]->getDensity();
	}


	particle->setVelocity(particle->getVelocity() + pressureForce * particle->getPressure() / particle->getDensity() * settings.timeStep);
}

void calculateViscosity(SphParticle* particle, std::vector<SphParticle*> neighbours, const SPHSettings& settings)
{
	glm::vec3 viscosityForce(0);
	for (int i = 0; i < neighbours.size(); i++)
	{
		glm::vec3 ab = (neighbours[i]->getPosition() + neighbours[i]->getVelocity() * settings.timeStep) - (particle->getPosition() + particle->getVelocity() * settings.timeStep) ;
		float dist = glm::length(ab);
		if (dist > settings.h + particle->getRadius()) continue;
		
		viscosityForce += (neighbours[i]->getVelocity() - particle->getVelocity()) / neighbours[i]->getDensity() * kernelFunc(settings.h + particle->getRadius(), dist) * settings.mass;
	}
	
	particle->setVelocity(particle->getVelocity() + viscosityForce * settings.viscosity * settings.timeStep);
}

