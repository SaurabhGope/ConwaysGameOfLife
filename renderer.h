#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "grid.h"

class Renderer {
public:
    static void Initialize(int screenWidth, int screenHeight);
    static void Shutdown();
    static void DrawGrid(const Grid& grid);
    static void ProcessInput(GLFWwindow* window, float deltaTime);
    
    static glm::mat4 GetViewMatrix();
    static glm::mat4 GetProjectionMatrix();

private:
    static void SetupCube();
    static void LoadShaders();

    static unsigned int VAO, VBO, shaderProgram;
    static glm::vec3 cameraPos;
    static glm::vec3 cameraFront;
    static glm::vec3 cameraUp;
    
    static int screenWidth;
    static int screenHeight;
};