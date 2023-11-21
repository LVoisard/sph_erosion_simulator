#include <algorithm>
#include <numeric>
#include <glm/gtx/norm.hpp>
#include <neighborTable.h>
#include <sph.h>
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


void parallelCalculateHashes(
    SphParticle* particles, size_t start, size_t end, const SPHSettings& settings)
{
    for (size_t i = start; i < end; i++) {
        SphParticle* particle = &particles[i];
        particle->setHash(getHash(getCell(particle, settings.h)));
    }
}

//Calculate the particles' density and pressure based on the neighbors. Can be computed in parallel
void parallelDensityAndPressures(
	SphParticle* particles, const size_t particleCount, const size_t start,
	const size_t end, const uint32_t* particleTable,
	const SPHSettings& settings)
{
	float massPoly6Product = settings.mass * settings.poly6;

	for (size_t piIndex = start; piIndex < end; piIndex++) {
		float pDensity = 0;
		SphParticle* pi = &particles[piIndex];
		glm::ivec3 cell = getCell(pi, settings.h);

		for (int x = -1; x <= 1; x++) {
			for (int y = -1; y <= 1; y++) {
				for (int z = -1; z <= 1; z++) {
					uint16_t cellHash = getHash(cell + glm::ivec3(x, y, z));
					uint32_t pjIndex = particleTable[cellHash];
					if (pjIndex == NO_PARTICLE) {
						continue;
					}
					while (pjIndex < particleCount) {
						if (pjIndex == piIndex) {
							pjIndex++;
							continue;
						}
						SphParticle* pj = &particles[pjIndex];
						if (pj->getHash() != cellHash) {
							break;
						}
						float dist2 = glm::length2(pj->getPosition() - pi->getPosition());
						if (dist2 < settings.h2) {
							pDensity += massPoly6Product * glm::pow(settings.h2 - dist2, 3); //kernel function
						}
						pjIndex++;
					}
				}
			}
		}

		// Include self density (as itself isn't included in neighbour)
		pi->setDensity(pDensity + settings.selfDens);

		// Calculate pressure
		float pPressure
			= settings.gasConstant * (pi->getDensity() - settings.restDensity);
		pi->setPressure(pPressure);
	}
}

void parallelForces(
	SphParticle* particles, const size_t particleCount, const size_t start,
	const size_t end, const uint32_t* particleTable,
	const SPHSettings& settings)
{
	for (size_t piIndex = start; piIndex < end; piIndex++) {
		SphParticle* pi = &particles[piIndex];
		pi->setForce(glm::vec3(0));
		glm::ivec3 cell = getCell(pi, settings.h);

		for (int x = -1; x <= 1; x++) {
			for (int y = -1; y <= 1; y++) {
				for (int z = -1; z <= 1; z++) {
					uint16_t cellHash = getHash(cell + glm::ivec3(x, y, z));
					uint32_t pjIndex = particleTable[cellHash];
					if (pjIndex == NO_PARTICLE) {
						continue;
					}
					while (pjIndex < particleCount) {
						if (pjIndex == piIndex) {
							pjIndex++;
							continue;
						}
						SphParticle* pj = &particles[pjIndex];
						if (pj->getHash() != cellHash) {
							break;
						}
						float dist2 = glm::length2(pj->getPosition() - pi->getPosition());
						if (dist2 < settings.h2) {
							//unit direction and length
							float dist = sqrt(dist2);
							glm::vec3 dir = glm::normalize(pj->getPosition() - pi->getPosition());

							//apply pressure force
							glm::vec3 pressureForce = -dir * settings.mass * (pi->getPressure() + pj->getPressure()) / (2 * pj->getDensity()) * settings.spikyGrad;
							pressureForce *= std::pow(settings.h - dist, 2);
							pi->setForce(pi->getForce() + pressureForce);

							//apply viscosity force
							glm::vec3 velocityDif = pj->getVelocity() - pi->getVelocity();
							glm::vec3 viscoForce = settings.viscosity * settings.mass * (velocityDif / pj->getDensity()) * settings.spikyLap * (settings.h - dist);
							pi->setForce(pi->getForce() + viscoForce);
						}
						pjIndex++;
					}
				}
			}
		}
	}
}


void parallelUpdateParticlePositions(
	SphParticle* particles, const size_t particleCount, const size_t start,
	const size_t end, const SPHSettings& settings, const float& deltaTime)
{

	float boxWidth = 8.f;
	float elasticity = 0.5f;

	for (size_t i = start; i < end; i++) {
		SphParticle* p = &particles[i];

		//calculate acceleration and velocity
		glm::vec3 acceleration = p->getForce() / p->getDensity() + glm::vec3(0, settings.g, 0);
		p->setVelocity(p->getVelocity() + acceleration * deltaTime);

		// Update position
		p->setPosition(p->getPosition() + p->getVelocity() * deltaTime);

		//The boundry & Terrain collison handling need to be implemented 
	
	}
}

void sortParticles(SphParticle* particles, const size_t& particleCount)
{
	std::sort(
		particles, particles + particleCount,
		[&](const SphParticle& i, const SphParticle& j) {
			return i.getHash() < j.getHash();
		}
	);
}



