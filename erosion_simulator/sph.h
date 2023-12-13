#ifndef SPH_SPH_H
#define SPH_SPH_H

#include "Sph_Particle.h"


struct SPHSettings
{
    SPHSettings(
        float mass, float restDensity, float pressureMultiplier, float nearPressureMultiplier, float viscosity,
        float h, float g, float sedimentSaturation, float timeStep);

    glm::mat4 sphereScale;
    float pressureMultiplier, surfaceTensionMultiplier, mass, h2,
          restDensity, viscosity, h, g, sedimentSaturation, timeStep;
};

float kernelFuncSmooth(float h2, float x2);

// this kernel is specific to pressure, because the smooth kernels have a vanishing gradient at the center
// this allows for partciles to be repulsed by eachother.
// https://matthias-research.github.io/pages/publications/sca03.pdf
float kernelFuncSpiky3(float h, float x);


// https://github.com/SebLague/Fluid-Sim
float kernelFuncSpiky2(float h, float x);


// https://matthias-research.github.io/pages/publications/sca03.pdf
float kernelFuncViscosity(float h, float dist);

void calculateDensity(SphParticle* particle, std::vector<SphParticle*> neighbours, const SPHSettings& settings);

void calculateSedimentDensity(SphParticle* particle, std::vector<SphParticle*> neighbours, const SPHSettings& settings);

void calculatePressureForce(SphParticle* particle, std::vector<SphParticle*> neighbours, const SPHSettings& settings);

void calculateViscosity(SphParticle* particle, std::vector<SphParticle*> neighbours, const SPHSettings& settings);

void calculateSufaceTension(SphParticle* particle, std::vector<SphParticle*> neighbours, const SPHSettings& settings);

#endif //SPH_SPH_H