#include "ui/UiLayer.hpp"

#include <array>
#include <algorithm>
#include <cstdio>
#include <imgui.h>

namespace
{
constexpr char kGridItems[] = "64^3\0" "96^3\0" "128^3\0" "192^3\0" "256^3\0" "\0";
constexpr char kBoundaryItems[] = "Fixed dead\0" "Toroidal\0" "\0";
constexpr char kGraphicsApiItems[] = "Vulkan\0" "OpenGL\0" "D3D12\0" "Metal (planned)\0" "WebGPU (planned)\0" "\0";
constexpr char kRenderModeItems[] = "Solid\0" "Wireframe\0" "Point cloud\0" "\0";
constexpr char kSeedItems[] = "Cube\0" "Sphere\0" "Line\0" "Noise\0" "\0";

int GraphicsApiIndex(GraphicsApi api)
{
    switch (api)
    {
    case GraphicsApi::Vulkan:
        return 0;
    case GraphicsApi::OpenGL:
        return 1;
    case GraphicsApi::D3D12:
        return 2;
    case GraphicsApi::Metal:
        return 3;
    case GraphicsApi::WebGPU:
        return 4;
    }

    return 1;
}

GraphicsApi GraphicsApiFromIndex(int index)
{
    constexpr std::array<GraphicsApi, 5> values{
        GraphicsApi::Vulkan,
        GraphicsApi::OpenGL,
        GraphicsApi::D3D12,
        GraphicsApi::Metal,
        GraphicsApi::WebGPU};
    return values[static_cast<size_t>(std::clamp(index, 0, static_cast<int>(values.size() - 1)))];
}

int RenderModeIndex(RenderMode mode)
{
    switch (mode)
    {
    case RenderMode::Solid:
        return 0;
    case RenderMode::Wireframe:
        return 1;
    case RenderMode::PointCloud:
        return 2;
    }

    return 0;
}

RenderMode RenderModeFromIndex(int index)
{
    constexpr std::array<RenderMode, 3> values{RenderMode::Solid, RenderMode::Wireframe, RenderMode::PointCloud};
    return values[static_cast<size_t>(std::clamp(index, 0, static_cast<int>(values.size() - 1)))];
}

int BoundaryModeIndex(BoundaryMode mode)
{
    return mode == BoundaryMode::Toroidal ? 1 : 0;
}

BoundaryMode BoundaryModeFromIndex(int index)
{
    return index == 1 ? BoundaryMode::Toroidal : BoundaryMode::FixedDead;
}

const char* SeedTypeName(SeedType type)
{
    switch (type)
    {
    case SeedType::Cube:
        return "Cube";
    case SeedType::Sphere:
        return "Sphere";
    case SeedType::Line:
        return "Line";
    case SeedType::Noise:
        return "Noise";
    case SeedType::Custom:
        return "Custom";
    }

    return "Unknown";
}
}

UiActions UiLayer::Draw(AppConfig& config, WeightedRuleSet& rules, const SimulationStats& simStats, const RendererStats& rendererStats)
{
    UiActions actions{};

    ImGui::Begin("Life3D Controls");

    ImGui::Text("Generation: %llu", static_cast<unsigned long long>(simStats.generation));
    ImGui::Text("Alive cells: %llu", static_cast<unsigned long long>(simStats.aliveCells));
    ImGui::Text("Step: %.3f ms", simStats.lastStepMs);
    ImGui::Separator();

    if (ImGui::Button(config.simulation.paused ? "Run" : "Pause"))
    {
        actions.togglePause = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Step"))
    {
        actions.singleStep = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset"))
    {
        actions.resetSimulation = true;
    }

    int steps = static_cast<int>(config.simulation.stepsPerSecond);
    if (ImGui::SliderInt("Steps/s", &steps, 1, 240))
    {
        actions.setStepsPerSecond = static_cast<uint32_t>(steps);
    }

    constexpr std::array<uint32_t, 5> sizes{64, 96, 128, 192, 256};
    int gridSize = 2;
    for (size_t i = 0; i < sizes.size(); ++i)
    {
        if (config.grid.dims.x == sizes[i])
        {
            gridSize = static_cast<int>(i);
            break;
        }
    }
    if (ImGui::Combo("Grid", &gridSize, kGridItems))
    {
        const uint32_t size = sizes[static_cast<size_t>(gridSize)];
        actions.resizeGrid = glm::uvec3{size, size, size};
    }

    int boundary = BoundaryModeIndex(config.grid.boundary);
    if (ImGui::Combo("Boundary", &boundary, kBoundaryItems))
    {
        actions.switchBoundaryMode = BoundaryModeFromIndex(boundary);
    }

    ImGui::Separator();
    int api = GraphicsApiIndex(config.render.api);
    if (ImGui::Combo("Graphics API", &api, kGraphicsApiItems))
    {
        actions.switchApi = GraphicsApiFromIndex(api);
    }

    int renderMode = RenderModeIndex(config.render.mode);
    if (ImGui::Combo("Render Mode", &renderMode, kRenderModeItems))
    {
        actions.switchRenderMode = RenderModeFromIndex(renderMode);
    }

    if (ImGui::Button("Reload Shaders"))
    {
        actions.reloadShaders = true;
    }

    ImGui::Text("Backend: %s", rendererStats.backendName.c_str());
    ImGui::Text("Device: %s", rendererStats.deviceName.c_str());
    ImGui::Text("Rendered cells: %llu", static_cast<unsigned long long>(rendererStats.renderedCells));
    if (!rendererStats.statusMessage.empty())
    {
        ImGui::TextWrapped("%s", rendererStats.statusMessage.c_str());
    }

    ImGui::Separator();
    ImGui::Text("Weighted Rule");
    bool ruleChanged = false;
    ruleChanged |= ImGui::SliderFloat("Birth Min", &rules.birth.min, 0.0f, 26.0f);
    ruleChanged |= ImGui::SliderFloat("Birth Max", &rules.birth.max, 0.0f, 26.0f);
    ruleChanged |= ImGui::SliderFloat("Survive Min", &rules.survive.min, 0.0f, 26.0f);
    ruleChanged |= ImGui::SliderFloat("Survive Max", &rules.survive.max, 0.0f, 26.0f);
    if (rules.birth.min > rules.birth.max)
    {
        std::swap(rules.birth.min, rules.birth.max);
        ruleChanged = true;
    }
    if (rules.survive.min > rules.survive.max)
    {
        std::swap(rules.survive.min, rules.survive.max);
        ruleChanged = true;
    }

    if (ImGui::TreeNode("Neighbor Weights"))
    {
        for (size_t i = 0; i < rules.neighborWeights.size(); ++i)
        {
            char label[32]{};
            std::snprintf(label, sizeof(label), "Weight %02zu", i);
            ruleChanged |= ImGui::SliderFloat(label, &rules.neighborWeights[i], -4.0f, 4.0f);
        }
        ImGui::TreePop();
    }

    if (ruleChanged)
    {
        actions.updateRules = rules;
    }

    ImGui::Separator();
    static int seedType = 3;
    static int seedSize = 16;
    static float seedDensity = 0.18f;
    static int deterministicSeed = 1;
    ImGui::Combo("Seed", &seedType, kSeedItems);
    ImGui::SliderInt("Seed Size", &seedSize, 2, 96);
    ImGui::SliderFloat("Density", &seedDensity, 0.01f, 1.0f);
    ImGui::InputInt("Seed Value", &deterministicSeed);
    if (ImGui::Button("Inject Seed"))
    {
        constexpr std::array<SeedType, 4> seedTypes{SeedType::Cube, SeedType::Sphere, SeedType::Line, SeedType::Noise};
        const auto dims = config.grid.dims;
        SeedRequest request{};
        request.type = seedTypes[static_cast<size_t>(std::clamp(seedType, 0, 3))];
        request.deterministicSeed = static_cast<uint32_t>(std::max(0, deterministicSeed));
        request.transform.size = glm::uvec3{
            static_cast<uint32_t>(seedSize),
            static_cast<uint32_t>(seedSize),
            static_cast<uint32_t>(seedSize)};
        request.transform.density = seedDensity;
        request.transform.position = glm::ivec3{
            static_cast<int32_t>(dims.x / 2u) - seedSize / 2,
            static_cast<int32_t>(dims.y / 2u) - seedSize / 2,
            static_cast<int32_t>(dims.z / 2u) - seedSize / 2};
        actions.injectSeed = request;
    }
    ImGui::SameLine();
    ImGui::Text("%s", SeedTypeName(static_cast<SeedType>(std::clamp(seedType, 0, 3))));

    ImGui::End();

    return actions;
}
