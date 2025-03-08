#include <iostream>
#include "grid.h"
#include "ansi_escapes.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int main(int argc, char* argv[])
{
	// Initialize GLFW
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW!" << std::endl;
		return -1;
	}
	//Printing the version of GLFW
	std::cout << "GLFW Initialized! Version: " << glfwGetVersionString() << std::endl;

	GLFWwindow* window = glfwCreateWindow(800, 600, "Conway's Game of Life", nullptr, nullptr);
    if (!window)
	{
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
	
	// Load GLAD (must be done after creating OpenGL context)
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD!" << std::endl;
		return -1;
	}
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	
	// GLM Test
    glm::vec3 position(1.0f, 2.0f, 3.0f);
    std::cout << "GLM Vector: (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
	// Main render loop
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// std::cout << "Conway's Game of Life \n";
	// std::cout << "Press the return key to display each generation\n";

	// // Wait for user
	// std::cin.get();

	// // Enable ANSI escape codes on Windows
	// SetupConsole();

	// // Grid for the first generation
	// Grid currentGeneration;

	// // Populate the cells at random
	// currentGeneration.Randomize();

	// while (true)
	// {
	// 	// Draw the current generation
	// 	currentGeneration.Draw();

	// 	// Wait for user to press the return key
	// 	std::cin.get();

	// 	// Grid for the next generation
	// 	Grid nextGeneration;

	// 	// Populate the cells in the next generation
	// 	Calculate(currentGeneration, nextGeneration);

	// 	// Update to the next generation
	// 	currentGeneration.Update(nextGeneration);
	// }

	// // Move cursor to bottom of screen
	// std::cout << "\x1b[" << 0 << ";" << ROW_MAX - 1 << "H";

	//Terminate GLFW
	glfwTerminate();
	return 0;
	// Restore console on Windows
	// RestoreConsole();
}
