#include "window.h"

#include <iostream>
#include <string>

Window::Window(int width, int height)
    :width(width), height(height)
{
    init();
}

void Window::init()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    glfwWindow = glfwCreateWindow(width, height, "Erosion Simulation", NULL, NULL);
    if (glfwWindow == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }

    glfwGetFramebufferSize(glfwWindow, &bufferWidth, &bufferHeight);
    glfwMakeContextCurrent(glfwWindow);

    glfwSetKeyCallback(glfwWindow, key_callback);
    glfwSetCursorPosCallback(glfwWindow, cursor_position_callback);
    glfwSetMouseButtonCallback(glfwWindow, mouse_button_callback);
    glfwSetScrollCallback(glfwWindow, scroll_callback);
    
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    float heightScale, widthScale;
    glfwGetWindowContentScale(glfwWindow, &widthScale, &heightScale);
    io.FontGlobalScale = heightScale;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(glfwWindow, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    glViewport(0, 0, bufferWidth, bufferHeight - ImGui::GetFrameHeight());    

    glfwSetWindowUserPointer(glfwWindow, this);
    glfwSetFramebufferSizeCallback(glfwWindow, framebuffer_size_callback);


}

void Window::updateInput()
{

    for (int i = 0; i < 1024; i++) 
    {
        keysDown[i] = false;
        keysUp[i] = false;
    }

    for (int i = 0; i < 8; i++)
    {
        mouseButtonsDown[i] = false;
        mouseButtonsUp[i] = false;
    }

    mouseDeltaX = 0;
    mouseDeltaY = 0;
    mouseScrollY = 0;
}


void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Window* currentWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

    if (action == GLFW_PRESS)
    {
        currentWindow->keysDown[key] = true;
        currentWindow->keys[key] = true;
    }
    else if (action == GLFW_RELEASE)
    {
        currentWindow->keysUp[key] = true;
        currentWindow->keys[key] = false;
    }
}

bool firstTime = true;
void Window::cursor_position_callback(GLFWwindow* window, double xPos, double yPos)
{
    Window* currentWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
    
    if (firstTime) {
        currentWindow->mousePosX = xPos;
        currentWindow->mousePosY = yPos;
        firstTime = false;
        return;
    }

    currentWindow->mouseDeltaX = xPos - currentWindow->mousePosX;
    currentWindow->mouseDeltaY = currentWindow->mousePosY - yPos;
    currentWindow->mousePosX = xPos;
    currentWindow->mousePosY = yPos;
}

void Window::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    Window* currentWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

    if (action == GLFW_PRESS)
    {
        currentWindow->mouseButtons[button] = true;
        currentWindow->mouseButtonsDown[button] = true;
    } 
    else if (action == GLFW_RELEASE)
    {
        currentWindow->mouseButtons[button] = false;
        currentWindow->mouseButtonsUp[button] = true;
    }
}

void Window::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    Window* currentWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

    currentWindow->mouseScrollY = yoffset;
}


void Window::Menu(SPHSettings* settings, SimulationParametersUI* params)
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            ImGui::MenuItem("Save Height Map", NULL, &showSaveMenu);
            if (params->showRegenButton) {
                if (ImGui::MenuItem("Regenerate Heightmap", NULL))
                {
                    params->regenerateHeightMapRequested = true;
                }
            }

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Tools"))
        {
            ImGui::MenuItem("Simulation Parameters", NULL, &showSimulationParameters);
            ImGui::EndMenu();
        }
        /*if (ImGui::BeginMenu("Debug")) 
        {
            if (ImGui::BeginMenu("Terrain Debug Modes"))
            {
                if(ImGui::MenuItem("Normal", NULL))
                {
                    model->terrainDebugMode = TerrainDebugMode::TERRAIN_NORMAL;
                }
                if (ImGui::MenuItem("Erosion Susceptibility", NULL))
                {
                    model->terrainDebugMode = TerrainDebugMode::TERRAIN_SUSCEPTIBILITY;
                }
                if (ImGui::MenuItem("Sediment Deposition", NULL))
                {
                    model->terrainDebugMode = TerrainDebugMode::TERRAIN_SEDIMENT_DEPOSITION;
                }
                if (ImGui::MenuItem("Invisible", NULL))
                {
                    model->terrainDebugMode = TerrainDebugMode::TERRAIN_INVISIBLE;
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Water Debug Modes"))
            {
                if (ImGui::MenuItem("Normal", NULL))
                {
                    model->waterDebugMode = WaterDebugMode::WATER_NORMAL;
                }
                if (ImGui::MenuItem("Velocity", NULL))
                {
                    model->waterDebugMode = WaterDebugMode::WATER_VELOCITY;
                }
                if (ImGui::MenuItem("Sediment Transport", NULL))
                {
                    model->waterDebugMode = WaterDebugMode::WATER_SEDIMENT_TRANSPORT;
                }
                if (ImGui::MenuItem("Invisible", NULL))
                {
                    model->waterDebugMode = WaterDebugMode::WATER_INVISIBLE;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }*/
        ImGui::EndMainMenuBar();
    }   

    if (showSimulationParameters) ShowSimulationParameters(settings, &showSimulationParameters);
    //if (showPaintBrushMenu) ShowPaintBrushMenu(model, params, &showPaintBrushMenu);
    //if (showSaveMenu) ShowSaveMenu(params, &showSaveMenu);
}

void Window::ShowSimulationParameters(SPHSettings* settings, bool *open)
{
    if (ImGui::Begin("Simulation Parameters", open))
    {
        ImGui::Spacing();
        ImGui::Text("Particle Parameters");

        ImGui::SliderFloat("Particle Mass", &settings->mass, 0.1, 10, "%.2f");
        ImGui::SliderFloat("Rest Density", &settings->restDensity, 1, 3000, "%.2f");
        ImGui::SliderFloat("Pressure Multiplier", &settings->pressureMultiplier, 1, 1000, "%.2f");
        ImGui::SliderFloat("Near Pressure Multiplier", &settings->nearPressureMultiplier, 1, 100, "%.2f");

        ImGui::SliderFloat("Viscosity", &settings->viscosity, 0.0f, 100.0f, "%.2f");
        ImGui::SliderFloat("Smoothing Radius", &settings->h, 0.001, 1, "%.3f");
        ImGui::SliderFloat("Gravity Constant", &settings->g, -9.8, 9.8, "%.1f");
        ImGui::SliderFloat("Time Step", &settings->timeStep, 0.001, 0.5f, "%.3f");

        ImGui::End();
    }
}

void Window::ShowSaveMenu(SimulationParametersUI* params, bool* open)
{
    if (ImGui::Begin("Save Heightmap", open))
    {
        ImGui::InputText("File Name", params->fileSaveName, 100);
        if (ImGui::Button("Save"))
        {
            params->saveHeightMapRequested = true;
            *open = false;
        }

        ImGui::End();
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------

void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    Window* currentWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
    currentWindow->width = width;
    currentWindow->height = height - ImGui::GetFrameHeight();
    currentWindow->bufferWidth = width;
    currentWindow->bufferHeight = height;
    
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, currentWindow->width, currentWindow->height);

}

