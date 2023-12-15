#pragma once
#include <glm/glm.hpp>
#include <vector>

enum class TerrainDebugMode
{
	TERRAIN_NORMAL,
	TERRAIN_SEDIMENT_DEPOSITION,
	TERRAIN_INVISIBLE,
	COUNT, // used to cycle through options
};

enum class WaterDebugMode
{
	WATER_NORMAL,
	WATER_STYLIZED,
	WATER_VELOCITY,
	WATER_SEDIMENT_TRANSPORT,
	WATER_INVISIBLE,
	COUNT, // used to cycle through options
};

enum class PaintMode
{
	WATER_ADD,
	WATER_REMOVE,
	TERRAIN_ADD,
	TERRAIN_REMOVE,
	WATER_SOURCE,
	COUNT, // used to cycle through options
};

struct WaterSource
{
	glm::vec3 position;
	float radius;
	float intensity;
};

struct ErosionModel
{
	bool isSimRunning = false;

	bool debugNeighbours = false;
	bool debugTerrainParticles = false;
	bool debugGrid = false;

	int rainIntensity = 1;
	int rainAmount = 1;

	bool isRaining = false;

	bool castRays = false;
	float brushRadius = 1.0f;
	float brushIntensity = 1.0f;
	PaintMode paintMode = PaintMode::WATER_ADD;
	WaterDebugMode waterDebugMode = WaterDebugMode::WATER_NORMAL;
	TerrainDebugMode terrainDebugMode = TerrainDebugMode::TERRAIN_NORMAL;	

	std::vector<WaterSource> waterSources = std::vector<WaterSource>(0);

	ErosionModel() {
		rainIntensity = 1;
		rainAmount = 1;

		waterSources = std::vector<WaterSource>(0);		
	}	

	void ToggleModelRunning()
	{
		isSimRunning = !isSimRunning;
		printf("Model is %s\n", isSimRunning ? "Enabled" : "Disabled");	
	}

	void ToggleModelRaining()
	{
		isRaining = !isRaining;
		printf("%s Rain\n", isRaining ? "Enabled" : "Disabled");
	}

	void TogglePaintMode()
	{
		int current = (int)paintMode;
		if (current >= (int)PaintMode::COUNT - 1)
			paintMode = static_cast<PaintMode>(0);
		else
			paintMode = static_cast<PaintMode>(current + 1);
	}

	void ToggleTerrainDebugMode()
	{
		int current = (int)terrainDebugMode;
		if (current >= (int)TerrainDebugMode::COUNT - 1)
			terrainDebugMode = static_cast<TerrainDebugMode>(0);
		else
			terrainDebugMode = static_cast<TerrainDebugMode>(current + 1);
		printf("Terrain Debugging mode %d Enabled\n", (int)terrainDebugMode);
	}

	void ToggleWaterDebugMode()
	{
		int current = (int)waterDebugMode;
		if (current >= (int)WaterDebugMode::COUNT - 1)
			waterDebugMode = static_cast<WaterDebugMode>(0);
		else
			waterDebugMode = static_cast<WaterDebugMode>(current + 1);
		printf("Water Debugging mode %d Enabled\n", (int)waterDebugMode);
	}
};