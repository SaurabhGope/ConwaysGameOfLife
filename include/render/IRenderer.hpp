#pragma once

#include <cstdint>
#include <span>
#include <memory>
#include <string>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include "core/AppConfig.hpp"

struct RendererInitInfo
{
    void* nativeWindowHandle = nullptr;
    glm::uvec2 framebufferSize{1280, 720};
    GraphicsApi requestedApi = GraphicsApi::OpenGL;
};

struct RendererFrameInput
{
    glm::mat4 viewProjection{1.0f};
    RenderMode mode = RenderMode::Solid;
};

struct RendererStats
{
    std::string backendName = "Unknown";
    std::string deviceName = "Unknown";
    std::string statusMessage;
    double cpuFrameMs = 0.0;
    double gpuFrameMs = 0.0;
    double simulationDispatchMs = 0.0;
    uint64_t drawCalls = 0;
    uint64_t renderedCells = 0;
    uint64_t bufferCapacity = 0;
    bool available = false;
};

class IRenderer
{
public:
    virtual ~IRenderer() = default;
    virtual bool Initialize(const RendererInitInfo& initInfo) = 0;
    virtual void UploadAliveCells(std::span<const uint32_t> aliveLinearIndices, glm::uvec3 dims) = 0;
    virtual void RenderFrame(const RendererFrameInput& input) = 0;
    virtual void Resize(uint32_t width, uint32_t height) = 0;
    virtual bool ReloadShaders() = 0;
    virtual RendererStats Stats() const = 0;
    virtual void Shutdown() = 0;
};

const char* GraphicsApiName(GraphicsApi api);
bool IsGraphicsApiSupported(GraphicsApi api);
std::unique_ptr<IRenderer> CreateRenderer(GraphicsApi api);
