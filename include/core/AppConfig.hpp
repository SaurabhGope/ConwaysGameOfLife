#pragma once

#include <cstdint>
#include <string>
#include <glm/vec3.hpp>

enum class GraphicsApi
{
    Vulkan,
    OpenGL,
    D3D12,
    Metal,
    WebGPU
};

enum class RenderMode
{
    Solid,
    Wireframe,
    PointCloud
};

enum class BoundaryMode
{
    FixedDead,
    Toroidal
};

struct GridConfig
{
    glm::uvec3 dims{128, 128, 128};
    BoundaryMode boundary = BoundaryMode::FixedDead;
};

struct SimulationConfig
{
    uint32_t stepsPerSecond = 30;
    bool paused = true;
    std::string rule = "B5/S456";
    float birthMin = 5.0f;
    float birthMax = 5.0f;
    float survivalMin = 4.0f;
    float survivalMax = 6.0f;
};

struct RenderConfig
{
    GraphicsApi api = GraphicsApi::Vulkan;
    RenderMode mode = RenderMode::Solid;
    bool enableFrustumCulling = false;
    bool enableChunking = false;
};

struct AppConfig
{
    GridConfig grid;
    SimulationConfig simulation;
    RenderConfig render;
};
