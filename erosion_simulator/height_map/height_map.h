#pragma once
#include <random>
#include <string>
#include "glm/glm.hpp"

class HeightMap
{
public:
	HeightMap();
	HeightMap(float minHeight, float maxHeight);
	void createProceduralHeightMap(int size, float random);
	void loadHeightMapFromFile(std::string);
	void loadHeightMapFromOBJFile(std::string, float heightDiff);
	void setHeightRange(float minHeight, float maxHeight);
	void setRandomRange(float random);
	float** getHeightMap() const { return heightMap; }
	int getWidth() const { return width; }
	int getLength() const { return length; }
	void printMap();
	void changeSeed() {mapGenerator.seed(seedDistr(seedGenerator)); regenerateHeightMap(); }
	void saveHeightMapPPM(std::string fileName);
	void saveHeightMapPPM(std::string fileName, float*** hmp);
	glm::vec3 getPositionAtIndex(int x, int y) const { return glm::vec3(x - offset.x, heightMap[x][y], y - offset.y); }
	float sampleHeightAtIndex(int x, int y) const { return heightMap[x][y]; }
	float getRGBA(int x, int y) const { return std::clamp(heightMap[x][y] + minHeight / maxHeight + minHeight, 0.0f, 1.0f); }
	int getMaxHeight() const { return maxHeight; }
	int getMinHeight() const { return minHeight; }
	float getMinX() const { return - offset.x; }
	float getMinZ() const { return - offset.y; }
	float getMaxX() const { return width - offset.x; }
	float getMaxZ() const { return length - offset.y; }
	int getHeight() const { return maxHeight - minHeight; }
	bool pointInBounds(float x, float z) const;
	glm::vec2 getOffset() const { return offset; }

	float** heightMap;

private:
	void generateHeightMap();
	void regenerateHeightMap();
	void squareStep(int chunkSize, int halfChunkSize);
	void diamondStep(int chunkSize, int halfChunkSize);


	int width;
	int length;

	float minHeight;
	float maxHeight;

	float random;

	glm::vec2 offset;

	std::default_random_engine seedGenerator;
	std::uniform_int_distribution<int> seedDistr;

	std::default_random_engine mapGenerator;
	std::uniform_real_distribution<> heightDistr;
	std::uniform_real_distribution<> randomDistr;
};

