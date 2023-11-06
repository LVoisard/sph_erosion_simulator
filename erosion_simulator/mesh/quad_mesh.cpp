#include "quad_mesh.h"

void QuadMesh::calculateVertices(HeightMap* map)
{
	for (int z = 0; z < length; z++) {
		for (int x = 0; x < width; x++) {
			Vertex v{};
			v.pos = map->getPositionAtIndex(x, z);
			v.normal = v.normal = glm::vec3(0.0f, 1.0f, 0.0f);
			v.uv = glm::vec2((float)x / width, (float)z / length) / (10.0f / width);
			vertices.push_back(v);
		}
	}
}

void QuadMesh::calculateVertices(float*** height)
{
	vertices.resize(width * length);

	for (int z = 0; z < length; z++) {
		for (int x = 0; x < width; x++) {
			Vertex v{};
			v.pos = glm::vec3(x - width / 2, (*height)[x][z], z - length / 2);
			v.normal = glm::vec3(0.0f, 1.0f, 0.0f);
			v.uv = glm::vec2((float)x / width, (float)z / length) / (10.0f / width);
			vertices[z * width + x] = v;
		}
	}
}

void QuadMesh::calculateIndices()
{
	indices.resize((width - 1) * (length - 1) * 6);
	int indicesIndex = 0;
	for (int z = 0; z < (length - 1); z++) {
		for (int x = 0; x < (width - 1); x++) {
			indices[indicesIndex + z * (width - 1) + x] = z * width + x; // 0
			indices[indicesIndex + z * (width - 1) + x + 1] = (z + 1) * width + x; // 2
			indices[indicesIndex + z * (width - 1) + x + 2] = z * width + x + 1; // 1

			// top triangle
			indices[indicesIndex + z * (width - 1) + x + 3] = (z + 1) * width + x + 1; // 3
			indices[indicesIndex + z * (width - 1) + x + 4] = z * width + x + 1; // 1
			indices[indicesIndex + z * (width - 1) + x + 5] = (z + 1) * width + x; // 2

			indicesIndex += 5;
		}
	}
}

void QuadMesh::calculateNormals()
{
	for (int y = 0; y < length; y++)
	{
		for (int x = 0; x < width; x++)
		{
			glm::vec3 center = vertices[y * width + x].pos;

			glm::vec3 top = y == length - 1 ? glm::vec3(0) : vertices[(y + 1) * width + x].pos;
			glm::vec3 bottom = y == 0 ? glm::vec3(0) : vertices[(y - 1) * width + x].pos;
			glm::vec3 right = x == width - 1 ? glm::vec3(0) : vertices[y * width + x + 1].pos;
			glm::vec3 left = x == 0 ? glm::vec3(0) : vertices[y * width + x - 1].pos;

			glm::vec3 v1 = normalize(right - center);
			glm::vec3 v2 = normalize(top - center);
			glm::vec3 v3 = normalize(left - center);
			glm::vec3 v4 = normalize(bottom - center);

			glm::vec3 normal1 = cross(v2, v1);
			glm::vec3 normal2 = cross(v3, v2);
			glm::vec3 normal3 = cross(v4, v3);
			glm::vec3 normal4 = cross(v1, v4);

			glm::vec3 normal = glm::vec3(0);

			if (x == 0 || x == width - 1 || y == 0 || y == length - 1)
			{
				if (top == glm::vec3(0))
				{
					if (left != glm::vec3(0))
						normal += normal3;
					if (right != glm::vec3(0))
						normal += normal4;
				}
				else if (bottom == glm::vec3(0))
				{
					if (left != glm::vec3(0))
						normal += normal2;
					if (right != glm::vec3(0))
						normal += normal1;
				}

				if (left == glm::vec3(0))
				{
					if (top != glm::vec3(0))
						normal += normal1;
					if (bottom != glm::vec3(0))
						normal += normal4;
				}
				else if (right == glm::vec3(0))
				{
					if (top != glm::vec3(0))
						normal += normal2;
					if (bottom != glm::vec3(0))
						normal += normal3;
				}
			}
			else
			{
				normal = normal1 + normal2 + normal3 + normal4;
			}

			vertices[y * width + x].normal = glm::normalize(normal);
		}
	}
}

QuadMesh::QuadMesh(int width, int length, Shader shader)
	:Mesh(shader), width(width), length(length)
{
}

QuadMesh::QuadMesh(int width, int length, std::vector<Vertex> vertices, std::vector<uint32_t> indices, Shader shader)
	:Mesh(vertices, indices, shader), width(width), length(length)
{
}

void QuadMesh::updateMeshFromMap(HeightMap* heightMap)
{
	clearData();
	calculateVertices(heightMap);
	calculateIndices();
	calculateNormals();
	update();
}

void QuadMesh::updateMeshFromHeights(float*** heights)
{
	clearData();
	calculateVertices(heights);
	calculateIndices();
	calculateNormals();
	update();
}