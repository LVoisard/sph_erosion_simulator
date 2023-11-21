#include <mutex>
#include <neighborTable.h>

uint32_t getHash(const glm::ivec3& cell)
{
    return (
        static_cast<uint32_t>(cell.x * 73856093)
        ^ static_cast<uint32_t>(cell.y * 19349663)
        ^ static_cast<uint32_t>(cell.z * 83492791)
        ) % TABLE_SIZE;
}

glm::ivec3 getCell(SphParticle* p, float h)
{
    return { p->getPosition().x / h, p->getPosition().y / h, p->getPosition().z / h};
}

uint32_t* createNeighborTable(
    SphParticle* sortedParticles, const size_t& particleCount)
{
    uint32_t* particleTable
        = (uint32_t*)malloc(sizeof(uint32_t) * TABLE_SIZE);
    for (size_t i = 0; i < TABLE_SIZE; ++i) {
        particleTable[i] = NO_PARTICLE;
    }

    uint32_t prevHash = NO_PARTICLE;
    for (size_t i = 0; i < particleCount; ++i) {
        uint32_t currentHash = sortedParticles[i].getHash();
        if (currentHash != prevHash) {
            particleTable[currentHash] = i;
            prevHash = currentHash;
        }
    }
    return particleTable;
}