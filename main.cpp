#include <iostream>
#include "grid.h"
#include "renderer.h"

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

int main() {
    // Initialize GLFW
    if(!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        return -1;
    }

    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Conway's Game of Life - Cellular Automata", NULL, NULL);
    if(!window) {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLAD
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD!" << std::endl;
        return -1;
    }

    // Initialize renderer
    Renderer::Initialize(SCR_WIDTH, SCR_HEIGHT);
    
    // Set up grid
    Grid gameGrid;
    gameGrid.Randomize();
    
    // Timing variables
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    float lastUpdate = 0.0f;

    // Main loop
    while(!glfwWindowShouldClose(window)) {
        // Calculate delta time
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Input handling
        Renderer::ProcessInput(window, deltaTime);

        // Update grid every 0.5 seconds
        if(currentFrame - lastUpdate >= 0.5f) {
			Grid nextGrid;
			Calculate(gameGrid, nextGrid);
			gameGrid.Update(nextGrid);
			lastUpdate = currentFrame;
		}

        // Render
        Renderer::DrawGrid(gameGrid);

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    Renderer::Shutdown();
    glfwTerminate();
    return 0;
}