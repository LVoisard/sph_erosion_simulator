#pragma once
#include <random>
#include <string>

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
	float** getHeightMap() { return heightMap; }
	int getWidth() { return width; }
	int getLength() { return length; }
	void printMap();
	void changeSeed() {mapGenerator.seed(seedDistr(seedGenerator)); regenerateHeightMap(); }
	void saveHeightMapPPM(std::string fileName);
	void saveHeightMapPPM(std::string fileName, float*** hmp);
	glm::vec3 getPositionAtIndex(int x, int y) { return glm::vec3(x - offset.x, heightMap[x][y], y - offset.y); }
	float sampleHeightAtIndex(int x, int y) { return heightMap[x][y]; }
	float sampleAtPosition(float x, float y);
	float getRGBA(int x, int y) { return std::clamp(heightMap[x][y] + minHeight / maxHeight + minHeight, 0.0f, 1.0f); }
	int getMaxHeight() { return maxHeight; }
	int getMinHeight() { return minHeight; }
	int getHeight() { return maxHeight - minHeight; }


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

