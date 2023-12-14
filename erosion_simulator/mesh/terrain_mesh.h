#pragma once
#include "quad_mesh.h"
#include "sph_particle.h"

class TerrainMesh :  public QuadMesh
{
public:
	TerrainMesh(int width, int length, float*** terrainHeights, Shader shader);
	TerrainMesh(int width, int length, HeightMap* heightMap, Shader shader);
	~TerrainMesh();	

	virtual void updateMeshFromHeights(float*** heights) override;
	void updateOriginalHeights();
	void updateOriginalHeights(float*** heights);
	virtual void init() override;

	glm::vec3 getNormalAtIndex(int x, int y) const;
	glm::vec3 getPositionAtIndex(int x, int y) const;
	float sampleHeightAtPosition(float x, float y) const;
	glm::vec3 sampleNormalAtPosition(float x, float y) const;
	glm::vec3 sampleWeightedNormalAtPosition(float x, float y) const;
	void modify_height(float, float, float);
	void modify_height_at_index(int, int, float);
private:

	float* originalHeights;
	glm::vec2 offset;
};

