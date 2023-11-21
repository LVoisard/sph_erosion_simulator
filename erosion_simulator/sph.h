#ifndef SPH_SPH_H
#define SPH_SPH_H

#include "Sph_Particle.h"


struct SPHSettings
{
    SPHSettings(
        float mass, float restDensity, float gasConst, float viscosity,
        float h, float g, float tension);

    glm::mat4 sphereScale;
    float poly6, spikyGrad, spikyLap, gasConstant, mass, h2, selfDens,
          restDensity, viscosity, h, g, tension, massPoly6Product;
};

/// Calculates and stores particle hashes.
void parallelCalculateHashes(
    SphParticle* particles, size_t start, size_t end, const SPHSettings& settings);

/// Sort particles in place by hash.
void sortParticles(SphParticle* particles, const size_t& particleCount);

/// Update attrs of particles in place.
void updateParticles(
    SphParticle* particles, glm::mat4* particleTransforms,
    const size_t particleCount, const SPHSettings& settings,
    float deltaTime, const bool onGPU);

#endif //SPH_SPH_H