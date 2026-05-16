#pragma once

#include <optional>
#include "core/AppConfig.hpp"
#include "render/IRenderer.hpp"
#include "seed/SeedTypes.hpp"
#include "sim/SimulationEngine.hpp"
#include "sim/RuleSet.hpp"

struct UiActions
{
    bool togglePause = false;
    bool singleStep = false;
    bool resetSimulation = false;
    bool reloadShaders = false;
    uint32_t setStepsPerSecond = 0;
    std::optional<glm::uvec3> resizeGrid;
    std::optional<SeedRequest> injectSeed;
    std::optional<GraphicsApi> switchApi;
    std::optional<RenderMode> switchRenderMode;
    std::optional<BoundaryMode> switchBoundaryMode;
    std::optional<WeightedRuleSet> updateRules;
};

class UiLayer
{
public:
    UiActions Draw(AppConfig& config, WeightedRuleSet& rules, const SimulationStats& simStats, const RendererStats& rendererStats);
};
