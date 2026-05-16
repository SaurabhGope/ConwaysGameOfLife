#include <memory>
#include <string_view>

#include "core/Logger.hpp"
#include "render/IRenderer.hpp"
#include "seed/SeedEngine.hpp"
#include "sim/Grid3D.hpp"
#include "sim/RuleSet.hpp"
#include "sim/SimulationEngine.hpp"

namespace
{
bool Require(bool condition)
{
    return condition;
}
}

int main()
{
    LoggerConfig loggerConfig{};
    loggerConfig.consoleEnabled = false;
    loggerConfig.fileEnabled = false;
    loggerConfig.minimumLevel = LogLevel::Trace;
    Logger::Instance().Initialize(loggerConfig);
    LIFE3D_LOG_INFO("test", "Smoke test logger initialized.");

    if (!Require(LogLevelFromString("warning") == LogLevel::Warn && LogLevelName(LogLevel::Critical) == std::string_view("CRITICAL")))
    {
        return 10;
    }

    const RuleSet rules = RuleSet::FromString("B5/S456", BoundaryMode::FixedDead);

    if (!Require(rules.birth[5] && rules.survive[4] && rules.survive[5] && rules.survive[6]))
    {
        return 1;
    }

    const RuleSet separatedRules = RuleSet::FromString("B5,10/S4,5,6", BoundaryMode::FixedDead);
    if (!Require(separatedRules.birth[5] && separatedRules.birth[10] && separatedRules.survive[6]))
    {
        return 2;
    }

    Grid3D grid({4, 4, 4});
    grid.Set(1, 1, 1, true);

    if (!Require(grid.Get(1, 1, 1) && grid.AliveCount() == 1))
    {
        return 3;
    }

    WeightedRuleSet weighted = WeightedRuleSet::FromRuleSet(rules);
    if (!Require(weighted.ShouldLive(false, 5.0f) && weighted.ShouldLive(true, 4.0f) && !weighted.ShouldLive(false, 4.0f)))
    {
        return 4;
    }

    SimulationEngine simulation(std::move(grid), weighted);
    simulation.Step();

    if (!Require(simulation.Stats().generation == 1))
    {
        return 5;
    }

    WeightedRuleSet toroidal{};
    toroidal.neighborWeights.fill(1.0f);
    toroidal.birth = {1.0f, 1.0f};
    toroidal.survive = {0.0f, 26.0f};
    toroidal.boundary = BoundaryMode::Toroidal;

    Grid3D wrapGrid({3, 3, 3});
    wrapGrid.Set(2, 0, 0, true);
    SimulationEngine wrapSimulation(std::move(wrapGrid), toroidal);
    wrapSimulation.Step();
    if (!Require(wrapSimulation.Current().Get(0, 0, 0)))
    {
        return 6;
    }

    SeedEngine seedEngine;
    SeedRequest noise{};
    noise.type = SeedType::Noise;
    noise.deterministicSeed = 42;
    noise.transform.size = {8, 8, 8};
    noise.transform.density = 0.25f;
    const auto firstMask = seedEngine.Preview(noise);
    const auto secondMask = seedEngine.Preview(noise);
    if (!Require(firstMask.has_value() && secondMask.has_value() && firstMask->voxels == secondMask->voxels))
    {
        return 7;
    }

    if (!Require(!IsGraphicsApiSupported(GraphicsApi::Vulkan) && IsGraphicsApiSupported(GraphicsApi::OpenGL)))
    {
        return 8;
    }

    const auto plannedRenderer = CreateRenderer(GraphicsApi::D3D12);
    if (!Require(plannedRenderer != nullptr && plannedRenderer->Stats().backendName == "D3D12" && !plannedRenderer->Stats().available))
    {
        return 9;
    }

    Logger::Instance().Shutdown();
    return 0;
}
