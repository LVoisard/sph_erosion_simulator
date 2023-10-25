#pragma once
#include "quad_mesh.h"

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

	glm::vec3 getNormalAtIndex(int x, int y);
	glm::vec3 getPositionAtIndex(int x, int y);
private:

	float* originalHeights;
};

