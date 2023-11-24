#include <algorithm>
#include <numeric>
#include <glm/gtx/norm.hpp>
#include <sph.h>
#include <iostream>
#define PI 3.14159265359f


SPHSettings::SPHSettings(
	float mass, float restDensity, float gasConst, float viscosity, float h,
	float g, float tension)
	: mass(mass)
	, restDensity(restDensity)
	, pressureMultiplier(gasConst)
	, viscosity(viscosity)
	, h(h)
	, g(g)
	, tension(tension)
{
	h2 = h * h;
	//sphereScale = glm::scale(glm::vec3(h / 2.f));
}

float kernelFunc(float h, float dist)
{
	float volume = (PI * pow(h, 4)) / 6;
	return std::max(0.0f, powf((h - dist),3) / volume);
}

float kernelFuncDerivative(float h, float dist)
{
	float scale = 12 / pow(h, 4) * PI;
	return std::max(0.0f, (h - dist) * scale);
}


void calculateDensity(SphParticle* particle, std::vector<SphParticle*> neighbours,
	const SPHSettings& settings)
{
	float density = 0;
	for (int i = 0; i < neighbours.size(); i++) {

		float dist = glm::length(neighbours[i]->getPosition() - particle->getPosition());
		if (dist <= settings.h) {
			density += settings.mass * kernelFunc(settings.h, dist); //kernel function
		}
	}

	// add particle self density
	density += settings.mass * kernelFunc(settings.h, 0);

	particle->setDensity(density);
}

void calculatePressure(SphParticle* particle, std::vector<SphParticle*> neighbours,
	const SPHSettings& settings)
{
	float pressure = 0;
	for (int i = 0; i < neighbours.size(); i++)
	{
		pressure += neighbours[i]->getDensity() - particle->getDensity();
	}
	particle->setPressure(pressure);
}

float densityToPressure(float density, const SPHSettings& settings)
{
	float d = density - settings.restDensity;
	return d * settings.pressureMultiplier;
}

void calculatePressureForce(SphParticle* particle, std::vector<SphParticle*> neighbours, const SPHSettings& settings)
{
	glm::vec3 pressureForce(0);
	for (int i = 0; i < neighbours.size(); i++)
	{
		glm::vec3 ab = neighbours[i]->getPosition() - particle->getPosition();
		float slope = kernelFuncDerivative(settings.h, ab.length());

		pressureForce += densityToPressure(neighbours[i]->getDensity(), settings) * glm::normalize(ab) * slope * settings.mass / neighbours[i]->getDensity();
	}

	particle->setForce(pressureForce);
}

