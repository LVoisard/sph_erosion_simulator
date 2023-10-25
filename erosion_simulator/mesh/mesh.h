#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "height_map/height_map.h"
#include "shader/shader.h"

struct Vertex 
{
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 uv;
	float height;
	glm::vec2 velocity;
	float currentSediment;
	Vertex()
	{}
	Vertex(glm::vec3 pos, glm::vec3 normal, glm::vec2 uv) : pos(pos), normal(normal), uv(uv)
	{}
};

class Mesh
{
public:
	Mesh(Shader shader);
	Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, Shader shader);
	~Mesh();
	virtual void init();
	void draw();

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	Shader shader;
protected:


	void update();
	void clearData();
	uint32_t VAO, VBO, EBO;
private:

};

