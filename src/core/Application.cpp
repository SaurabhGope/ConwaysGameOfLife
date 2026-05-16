#include "core/Application.hpp"

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <memory>
#include <utility>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "camera/Camera.hpp"
#include "core/Logger.hpp"
#include "platform/Window.hpp"
#include "render/IRenderer.hpp"
#include "seed/SeedEngine.hpp"
#include "sim/RuleSet.hpp"
#include "sim/SimulationEngine.hpp"
#include "ui/UiLayer.hpp"

namespace
{
Grid3D CreateInitialGrid(glm::uvec3 dims, SeedEngine& seedEngine)
{
    Grid3D grid(dims);
    SeedRequest request{};
    request.type = SeedType::Noise;
    request.deterministicSeed = 1;
    request.transform.size = glm::uvec3{
        std::max(4u, dims.x / 4u),
        std::max(4u, dims.y / 4u),
        std::max(4u, dims.z / 4u)};
    request.transform.position = glm::ivec3{
        static_cast<int32_t>(dims.x / 2u - request.transform.size.x / 2u),
        static_cast<int32_t>(dims.y / 2u - request.transform.size.y / 2u),
        static_cast<int32_t>(dims.z / 2u - request.transform.size.z / 2u)};
    request.transform.density = 0.14f;
    seedEngine.Apply(request, grid);
    LIFE3D_LOG_INFO(
        "simulation",
        "Initial grid seeded. dims={}x{}x{} seed_size={}x{}x{} density={:.2f} alive={}",
        dims.x,
        dims.y,
        dims.z,
        request.transform.size.x,
        request.transform.size.y,
        request.transform.size.z,
        request.transform.density,
        grid.AliveCount());
    return grid;
}

std::unique_ptr<IRenderer> CreateInitializedRenderer(AppConfig& config, Window& window)
{
    RendererInitInfo init{};
    init.nativeWindowHandle = window.NativeHandle();
    init.framebufferSize = window.FramebufferSize();
    init.requestedApi = config.render.api;

    auto renderer = CreateRenderer(config.render.api);
    if (renderer->Initialize(init))
    {
        LIFE3D_LOG_INFO("render", "Renderer initialized with requested API {}.", GraphicsApiName(config.render.api));
        return renderer;
    }

    LIFE3D_LOG_WARN(
        "render",
        "Requested renderer {} is unavailable. Falling back to OpenGL.",
        GraphicsApiName(config.render.api));
    config.render.api = GraphicsApi::OpenGL;
    init.requestedApi = config.render.api;
    renderer = CreateRenderer(config.render.api);
    if (renderer->Initialize(init))
    {
        LIFE3D_LOG_INFO("render", "Fallback renderer initialized with API {}.", GraphicsApiName(config.render.api));
        return renderer;
    }

    LIFE3D_LOG_CRITICAL("render", "Failed to initialize fallback OpenGL renderer.");
    return nullptr;
}
}

Application::Application(AppConfig config)
    : m_config(std::move(config))
{
}

int Application::Run()
{
    if (!glfwInit())
    {
        LIFE3D_LOG_CRITICAL("platform", "glfwInit failed.");
        return EXIT_FAILURE;
    }

    LIFE3D_LOG_INFO("platform", "GLFW initialized.");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    Window window;
    if (!window.Create(1280, 720, "Life3D GPU Cellular Automata"))
    {
        LIFE3D_LOG_CRITICAL("platform", "Window creation failed.");
        glfwTerminate();
        return EXIT_FAILURE;
    }

    window.MakeContextCurrent();
    glfwSwapInterval(1);
    LIFE3D_LOG_INFO("platform", "Window and OpenGL context created.");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(window.NativeHandle()), true);
    ImGui_ImplOpenGL3_Init("#version 330");
    LIFE3D_LOG_INFO("ui", "ImGui initialized.");

    SeedEngine seedEngine;
    WeightedRuleSet rules = WeightedRuleSet::FromConfig(m_config.simulation, m_config.grid.boundary);
    auto simulation = std::make_unique<SimulationEngine>(CreateInitialGrid(m_config.grid.dims, seedEngine), rules);
    auto renderer = CreateInitializedRenderer(m_config, window);
    if (renderer == nullptr)
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        window.Destroy();
        glfwTerminate();
        return EXIT_FAILURE;
    }

    Camera camera;
    camera.SetPosition(glm::vec3{0.0f, 0.0f, 5.0f});

    UiLayer ui;
    auto lastFrame = std::chrono::high_resolution_clock::now();
    double simulationAccumulator = 0.0;

    while (!window.ShouldClose())
    {
        window.PollEvents();

        const auto now = std::chrono::high_resolution_clock::now();
        const double deltaSeconds = std::chrono::duration<double>(now - lastFrame).count();
        lastFrame = now;

        const auto framebuffer = window.FramebufferSize();
        renderer->Resize(framebuffer.x, framebuffer.y);
        if (framebuffer.y != 0)
        {
            camera.SetPerspective(1.0471976f, static_cast<float>(framebuffer.x) / static_cast<float>(framebuffer.y), 0.1f, 2000.0f);
        }

        if (!m_config.simulation.paused)
        {
            simulationAccumulator += deltaSeconds;
            const double stepInterval = 1.0 / static_cast<double>(std::max(1u, m_config.simulation.stepsPerSecond));
            uint32_t stepsThisFrame = 0;
            while (simulationAccumulator >= stepInterval && stepsThisFrame < 8u)
            {
                simulation->Step();
                simulationAccumulator -= stepInterval;
                ++stepsThisFrame;
            }
        }

        const auto aliveCells = simulation->BuildAliveLinearIndexList();
        renderer->UploadAliveCells(aliveCells, simulation->Current().Dims());

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        UiActions actions = ui.Draw(m_config, rules, simulation->Stats(), renderer->Stats());

        ImGui::Render();
        renderer->RenderFrame({camera.ViewProjectionMatrix(), m_config.render.mode});
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        window.SwapBuffers();

        if (actions.togglePause)
        {
            m_config.simulation.paused = !m_config.simulation.paused;
            LIFE3D_LOG_INFO("simulation", "Simulation {}.", m_config.simulation.paused ? "paused" : "running");
        }

        if (actions.singleStep)
        {
            simulation->Step();
            LIFE3D_LOG_DEBUG("simulation", "Single step requested. generation={}", simulation->Stats().generation);
        }

        if (actions.setStepsPerSecond != 0u)
        {
            m_config.simulation.stepsPerSecond = actions.setStepsPerSecond;
            LIFE3D_LOG_INFO("simulation", "Steps per second set to {}.", m_config.simulation.stepsPerSecond);
        }

        if (actions.switchRenderMode.has_value())
        {
            m_config.render.mode = actions.switchRenderMode.value();
            LIFE3D_LOG_INFO("render", "Render mode changed to {}.", static_cast<int>(m_config.render.mode));
        }

        if (actions.switchBoundaryMode.has_value())
        {
            m_config.grid.boundary = actions.switchBoundaryMode.value();
            rules.boundary = m_config.grid.boundary;
            simulation->SetRules(rules);
            LIFE3D_LOG_INFO("simulation", "Boundary mode changed to {}.", static_cast<int>(m_config.grid.boundary));
        }

        if (actions.updateRules.has_value())
        {
            rules = actions.updateRules.value();
            rules.boundary = m_config.grid.boundary;
            simulation->SetRules(rules);
            LIFE3D_LOG_DEBUG(
                "simulation",
                "Weighted rules updated. birth=[{:.2f}, {:.2f}] survive=[{:.2f}, {:.2f}]",
                rules.birth.min,
                rules.birth.max,
                rules.survive.min,
                rules.survive.max);
        }

        if (actions.reloadShaders)
        {
            const bool reloaded = renderer->ReloadShaders();
            LIFE3D_LOG_INFO("render", "Shader reload {}.", reloaded ? "succeeded" : "failed");
        }

        if (actions.switchApi.has_value())
        {
            LIFE3D_LOG_INFO("render", "Switching renderer to {}.", GraphicsApiName(actions.switchApi.value()));
            renderer->Shutdown();
            m_config.render.api = actions.switchApi.value();
            renderer = CreateInitializedRenderer(m_config, window);
            if (renderer == nullptr)
            {
                break;
            }
        }

        if (actions.resetSimulation)
        {
            simulation->Reset(CreateInitialGrid(m_config.grid.dims, seedEngine));
            LIFE3D_LOG_INFO("simulation", "Simulation reset.");
        }

        if (actions.resizeGrid.has_value())
        {
            m_config.grid.dims = actions.resizeGrid.value();
            simulation = std::make_unique<SimulationEngine>(CreateInitialGrid(m_config.grid.dims, seedEngine), rules);
            LIFE3D_LOG_INFO(
                "simulation",
                "Grid resized to {}x{}x{}.",
                m_config.grid.dims.x,
                m_config.grid.dims.y,
                m_config.grid.dims.z);
        }

        if (actions.injectSeed.has_value())
        {
            Grid3D edited = simulation->Current();
            seedEngine.Apply(actions.injectSeed.value(), edited);
            simulation->Reset(std::move(edited));
            LIFE3D_LOG_INFO("simulation", "Seed injected. alive={}", simulation->Stats().aliveCells);
        }
    }

    LIFE3D_LOG_INFO("app", "Application shutdown started.");
    renderer->Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    window.Destroy();
    glfwTerminate();
    LIFE3D_LOG_INFO("app", "Application shutdown complete.");
    return EXIT_SUCCESS;
}
