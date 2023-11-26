#ifndef SPH_SPH_H
#define SPH_SPH_H

#include "Sph_Particle.h"


struct SPHSettings
{
    SPHSettings(
        float mass, float restDensity, float pressureMultiplier, float nearPressureMultiplier, float viscosity,
        float h, float g, float timeStep);

    glm::mat4 sphereScale;
    float pressureMultiplier, nearPressureMultiplier, mass, h2,
          restDensity, viscosity, h, g, timeStep;
};

void calculateDensity(SphParticle* particle, std::vector<SphParticle*> neighbours, const SPHSettings& settings);

void calculatePressureForce(SphParticle* particle, std::vector<SphParticle*> neighbours, const SPHSettings& settings);

void calculateViscosity(SphParticle* particle, std::vector<SphParticle*> neighbours, const SPHSettings& settings);


#endif //SPH_SPH_H