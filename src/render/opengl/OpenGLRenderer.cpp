#include "render/opengl/OpenGLRenderer.hpp"

#include <algorithm>
#include <chrono>
#include <GLFW/glfw3.h>

bool OpenGLRenderer::Initialize(const RendererInitInfo&)
{
    m_initialized = true;
    m_stats = RendererStats{};
    m_stats.backendName = "OpenGL";
    m_stats.deviceName = "Current OpenGL context";
    m_stats.statusMessage = "Compatibility rendering backend active. GPU simulation backends are staged next.";
    m_stats.available = true;
    glEnable(GL_DEPTH_TEST);
    return true;
}

void OpenGLRenderer::UploadAliveCells(std::span<const uint32_t> aliveLinearIndices, glm::uvec3)
{
    m_stats.renderedCells = aliveLinearIndices.size();
    m_stats.bufferCapacity = std::max<uint64_t>(m_stats.bufferCapacity, aliveLinearIndices.size());
}

void OpenGLRenderer::RenderFrame(const RendererFrameInput& input)
{
    if (!m_initialized)
    {
        return;
    }

    const auto start = std::chrono::high_resolution_clock::now();

    const float activity = static_cast<float>(std::min<uint64_t>(m_stats.renderedCells, 200000ull)) / 200000.0f;
    glClearColor(0.03f + activity * 0.08f, 0.04f + activity * 0.18f, 0.07f + activity * 0.20f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (input.mode == RenderMode::Wireframe)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    m_stats.drawCalls = 1;
    const auto end = std::chrono::high_resolution_clock::now();
    m_stats.cpuFrameMs = std::chrono::duration<double, std::milli>(end - start).count();
}

void OpenGLRenderer::Resize(uint32_t width, uint32_t height)
{
    if (!m_initialized)
    {
        return;
    }

    glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
}

RendererStats OpenGLRenderer::Stats() const
{
    return m_stats;
}

bool OpenGLRenderer::ReloadShaders()
{
    m_stats.statusMessage = "OpenGL shader reload requested; fixed-function compatibility path is active.";
    return true;
}

void OpenGLRenderer::Shutdown()
{
    m_initialized = false;
    m_stats.available = false;
}
