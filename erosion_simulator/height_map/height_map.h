#pragma once
#include <random>
#include <string>

class HeightMap
{
public:
	HeightMap();
	HeightMap(double minHeight, double maxHeight);
	void createProceduralHeightMap(int size, double random);
	void loadHeightMapFromFile(std::string);
	void loadHeightMapFromOBJFile(std::string, float heightDiff);
	void setHeightRange(double minHeight, double maxHeight);
	void setRandomRange(double random);
	float** getHeightMap() const { return heightMap; }
	int getWidth() const { return width; }
	int getLength() const { return length; }
	void printMap();
	void changeSeed() {mapGenerator.seed(seedDistr(seedGenerator)); regenerateHeightMap(); }
	void saveHeightMapPPM(std::string fileName);
	void saveHeightMapPPM(std::string fileName, float*** hmp);
	double samplePoint(int x, int y) const { return heightMap[x][y]; }
	double samplePoint(float x, float y) const;
	double getRGBA(int x, int y) { return std::clamp((double)heightMap[x][y] + minHeight / (double)maxHeight + minHeight, 0.0, 1.0); }
	int getMaxHeight() const { return maxHeight; }
	int getMinHeight() const { return minHeight; }
	int getHeight() const { return maxHeight - minHeight; }


	float** heightMap;

private:
	void init();
	void generateHeightMap();
	void regenerateHeightMap();
	void squareStep(int chunkSize, int halfChunkSize);
	void diamondStep(int chunkSize, int halfChunkSize);


	int width;
	int length;

	double minHeight;
	double maxHeight;

	double random;

	std::default_random_engine seedGenerator;
	std::uniform_int_distribution<int> seedDistr;

	std::default_random_engine mapGenerator;
	std::uniform_real_distribution<> heightDistr;
	std::uniform_real_distribution<> randomDistr;
};

