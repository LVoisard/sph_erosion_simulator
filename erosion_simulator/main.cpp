#include "simulation_parameters_ui.h"
#include "height_map/height_map.h"
#include "mesh/terrain_mesh.h"
#include "window/window.h"
#include "shader/shader.h"
#include "camera/camera.h"
#include "skybox/skybox.h"
#include <glm/glm.hpp>
#include "texture/texture.h"
#include <string>
#include <vector>
#include <chrono>
#include <mesh/water_mesh.h>
#include "mesh/sphere.h"
#include "particle.h"
#include "particle_generator.h"
#include "external/simpleppm.h"

#include <iostream>

#include <random>
#include "imgui.h"

#define GLM_FORCE_RADIANS
const float GRAVITY_ACCELERATION = 9.807f;

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

float waterflowRate = 0.1f;

Window window(SCR_WIDTH, SCR_HEIGHT);
Shader mainShader("shaders/main.vert", "shaders/main.frag");
Shader waterShader("shaders/water.vert", "shaders/water.frag");
Shader defaultShader("shaders/default.vert", "shaders/default.frag");
Shader boundaryParticleShader("shaders/boundary-particle.vert", "shaders/boundary-particle.frag");
Camera camera(&window, 5.0f, .25f, .5f);

Texture grassTexture("textures/grass.jpg");
Texture sandTexture("textures/sand.jpg");
Texture rockTexture("textures/rock.jpg");
Texture waterNormalTexture("textures/water-normal-map.jpg");

std::vector<std::string> skyboxFacesLocation{
	"textures/skybox/px.png",
	"textures/skybox/nx.png",
	"textures/skybox/py.png",
	"textures/skybox/ny.png",
	"textures/skybox/pz.png",
	"textures/skybox/nz.png"
};

Skybox skybox(skyboxFacesLocation);

// Max is 4096

// max size for cpu computations is 512,
// 256 is better
//2 ^ n
HeightMap map;

TerrainMesh* terrainMesh;
WaterMesh* waterMesh;
Sphere* sphere;
Sphere* boundaryParticleSphere;
ParticleGenerator* sphParticles;

ErosionModel erosionModel;
SimulationParametersUI* simParams;


std::default_random_engine gen;
std::uniform_int_distribution<> distr;

float fov = 90.0f;
glm::vec3 cursorOverPosition = glm::vec3(INT_MIN);

float timePast = 0.0f;

void raycastThroughScene()
{
	float pixelSize = (2 * tanf(fov) / 2 / window.getHeight());
	glm::vec3 A = camera.getPosition() - camera.getLookAt();
	glm::vec3 up = camera.getUp();
	glm::vec3 right = camera.getRight();
	glm::vec3 B = (A + tanf(fov) / 2 * up);
	glm::vec3 C = B - (window.getWidth() / 2 * pixelSize * right);

	glm::vec3 direction = glm::normalize(
		C +
		((window.getMousePosX() * pixelSize + pixelSize / 2.0f) * camera.getRight()) -
		((window.getMousePosY() * pixelSize + pixelSize / 2.0f) * camera.getUp() + camera.getPosition()));

	cursorOverPosition = glm::vec3(INT_MIN);

	for (int i = 0; i < terrainMesh->indices.size(); i += 3)
	{
		glm::vec3 triA = terrainMesh->vertices[terrainMesh->indices[i]].pos;
		glm::vec3 triB = terrainMesh->vertices[terrainMesh->indices[i + 1]].pos;
		glm::vec3 triC = terrainMesh->vertices[terrainMesh->indices[i + 2]].pos;
		glm::vec3 normal = glm::normalize(glm::cross(triC - triB, triA - triB));
		float t = glm::dot(triA - camera.getPosition(), normal) / glm::dot(direction, normal);

		glm::vec3 P = camera.getPosition() + t * direction;

		//check if in triangle
		float Sbc = glm::dot(glm::cross(triC - triB, P - triB), normal);
		float Sca = glm::dot(glm::cross(triA - triC, P - triC), normal);
		float Sab = glm::dot(glm::cross(triB - triA, P - triA), normal);

		if (Sbc >= 0 && Sca >= 0 && Sab >= 0)
		{
			cursorOverPosition = P;
			return;
		}
	}
}

void HandleHeightmapResets()
{
	//if (simParams->regenerateHeightMapRequested || window.getKeyDown(GLFW_KEY_R)) {
	//	simParams->regenerateHeightMapRequested = false;
	//	map.changeSeed();
	//	terrainMesh->updateMeshFromMap(&map);
	//	terrainMesh->updateOriginalHeights();
	//	// resetModel();
	//}

	// DOESNT WORK FOR NOW

	if (simParams->saveHeightMapRequested)
	{
		simParams->saveHeightMapRequested = false;
		std::vector<double> buffer(3 * map.getWidth() * map.getLength());

		for (int y = 0; y < map.getLength(); y++) {
			for (int x = 0; x < map.getWidth(); x++) {
				double color = std::clamp((double)(terrainMesh->getPositionAtIndex(x, y).y + map.getMinHeight()) / (double)(map.getMaxHeight() + map.getMinHeight()), 0.0, 1.0);
				buffer[3 * y * map.getWidth() + 3 * x + 0] = color;
				buffer[3 * y * map.getWidth() + 3 * x + 1] = color;
				buffer[3 * y * map.getWidth() + 3 * x + 2] = color;
			}
		}
		std::string fileName = std::string(simParams->fileSaveName) + ".ppm";
		save_ppm(fileName, buffer, map.getWidth(), map.getLength());
		buffer.clear();
	}


}
void HandleKeyboardInputs()
{
	if (erosionModel.castRays && erosionModel.isSimRunning && window.getMouseButton(GLFW_MOUSE_BUTTON_LEFT)) {
		if (erosionModel.paintMode == PaintMode::WATER_ADD && cursorOverPosition != glm::vec3(INT_MIN))
		{
			sphParticles->addParticles(cursorOverPosition, erosionModel.brushRadius, erosionModel.brushIntensity);
		}
	}
}

void HandleCamera(float deltaTime)
{
	if (window.getMouseButtonDown(GLFW_MOUSE_BUTTON_RIGHT))
	{
		camera.toggleViewMode();
		if (camera.inFreeView())
			cursorOverPosition = glm::vec3(INT_MIN);
	}

	camera.update(deltaTime);

	if (erosionModel.castRays && !camera.inFreeView())
		raycastThroughScene();
}

void UpdateShaders(glm::mat4& view, glm::mat4& proj, glm::mat4& model, float& deltaTime)
{
	skybox.DrawSkybox(view, proj);

	mainShader.use();
	mainShader.setMat4("model", model);
	mainShader.setMat4("view", view);
	mainShader.setMat4("projection", proj);
	mainShader.setUniformVector3("cursorOverTerrainPos", cursorOverPosition);

	mainShader.setTexture("texture0", 0);
	grassTexture.use(GL_TEXTURE0);
	mainShader.setTexture("texture1", 1);
	sandTexture.use(GL_TEXTURE1);
	mainShader.setTexture("texture2", 2);
	rockTexture.use(GL_TEXTURE2);

	terrainMesh->draw();

	mainShader.stop();

	waterShader.use();
	waterShader.setMat4("view", view);
	waterShader.setMat4("projection", proj);
	waterShader.setUniformVector3("viewerPosition", camera.getPosition());
	waterShader.setUniformFloat("deltaTime", &deltaTime);
	waterNormalTexture.use();
	waterShader.setTexture("texture0", GL_TEXTURE0);
	waterShader.setUniformInt("waterDebugMode", (int)erosionModel.waterDebugMode);

	sphParticles->drawParticles();
	waterShader.stop();

	if (false) {
		defaultShader.use();
		defaultShader.setMat4("view", view);
		defaultShader.setMat4("projection", proj);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		sphParticles->drawGridDebug();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		defaultShader.stop();
	}

	if (true) {
		boundaryParticleShader.use();
		boundaryParticleShader.setMat4("view", view);
		boundaryParticleShader.setMat4("projection", proj);
		sphParticles->drawTerrainParticles();
		boundaryParticleShader.stop();
	}

}

int main(int argc, char* argv[])
{
	if (argc <= 1)
	{
		printf("Invalid arguments, possible commands: \n");
		printf("default (n (1 - 11)) (randomness factor(0-4)) minH maxH \n");
		printf("heightmap (filepath) \n");
		printf("obj (filepath) (slopeHeight)\n");
		return -1;
	}

	for (int i = 0; i < argc; i++)
	{
		printf(argv[i]);
	}
	if (std::string(argv[1]) == "default")
	{
		map.setHeightRange(std::stoi(argv[4]), std::stoi(argv[5]));
		map.createProceduralHeightMap(std::pow(2, std::stoi(argv[2])), std::stoi(argv[3]));
	}
	else if (std::string(argv[1]) == "heightmap")
	{
		map.setHeightRange(std::stoi(argv[3]), std::stoi(argv[4]));
		map.loadHeightMapFromFile(std::string(argv[2]));
	}
	else if (std::string(argv[1]) == "obj")
	{
		map.setHeightRange(std::stoi(argv[3]), std::stoi(argv[4]));
		map.loadHeightMapFromOBJFile(std::string(argv[2]), argc == 6 ? std::stoi(argv[5]) : 0);
	}

	distr = std::uniform_int_distribution(0, map.getWidth() * map.getLength());
	simParams = new SimulationParametersUI(std::string(argv[1]) == "default");
	// initModel();

	terrainMesh = new TerrainMesh(map.getWidth(), map.getLength(), &map.heightMap, mainShader);

	float particleRadius = 0.05;
	sphere = new Sphere(glm::vec3(0), particleRadius, waterShader);
	boundaryParticleSphere = new Sphere(glm::vec3(0), particleRadius, boundaryParticleShader);

	terrainMesh->init();
	sphere->init();
	boundaryParticleSphere->init();

	float terrainSpacing = 1;

	// this is cubed (3 = 27 in one cube)
	int numInOneCell = 1;
	float h = 0.2;
	SPHSettings settings = SPHSettings(1, 880, 580, 0.25, 0.01, h, -9.8f, 0.032f);
	sphParticles = new ParticleGenerator(defaultShader, sphere, boundaryParticleSphere, &map, terrainMesh, terrainSpacing, h, particleRadius, numInOneCell, &settings);

	glm::mat4 proj = glm::mat4(1.0f);
	proj = glm::perspective(glm::radians(fov), window.getAspectRatio(), 0.1f, 1000.0f);
	auto currentTime = std::chrono::high_resolution_clock::now();
	float fpsTimer = 0;
	while (!window.shouldWindowClose())
	{
		auto newTime = std::chrono::high_resolution_clock::now();
		float deltaTime =
			std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
		currentTime = newTime;
		timePast += deltaTime;

		if (fpsTimer > 0.25f) {
			window.updateTitle(std::string("Erosion Simulation | FPS: " + std::to_string((float)1 / deltaTime)).c_str());
			fpsTimer = 0;
		}
		fpsTimer += deltaTime;

		HandleHeightmapResets();
		// stop taking input
		if (!window.showSaveMenu) {
			HandleKeyboardInputs();
			HandleCamera(deltaTime);
		}



		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = camera.getViewMatrix();

		if (erosionModel.isSimRunning)
			sphParticles->updateParticles(deltaTime, timePast);

		// drawing
		UpdateShaders(view, proj, model, deltaTime);

		window.Menu(&erosionModel , &settings, simParams);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		ImGui::EndFrame();

		window.swapBuffers();
		window.updateInput();
		window.pollEvents();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}