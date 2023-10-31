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
	Mesh();
	Mesh(Shader& shader);
	Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, Shader& shader);
	~Mesh();
	virtual void init();
	virtual void draw();
	virtual void drawInstanced(int amount);

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	Shader shader;
	void update();
	uint32_t getVAO() { return VAO; }
protected:


	void clearData();
	uint32_t VAO, VBO, EBO;
private:

};

