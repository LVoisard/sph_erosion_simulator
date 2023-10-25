#pragma once
#include "mesh/mesh.h"
class QuadMesh : public Mesh
{
public:
	QuadMesh(int width, int length, Shader shader);
	QuadMesh(int width, int length, std::vector<Vertex> vertices, std::vector<uint32_t> indices, Shader shader);

	void updateMeshFromMap(HeightMap* heightMap);
	virtual void updateMeshFromHeights(float*** heights);
protected:

	int width, length;
	virtual void calculateVertices(HeightMap* map);
	virtual void calculateVertices(float*** height);
	virtual void calculateIndices();
	virtual void calculateNormals();
private:
};

