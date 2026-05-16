#include "render/IRenderer.hpp"

#include <utility>
#include "render/opengl/OpenGLRenderer.hpp"

namespace
{
class UnsupportedRenderer final : public IRenderer
{
public:
    explicit UnsupportedRenderer(GraphicsApi api)
        : m_api(api)
    {
        m_stats.backendName = GraphicsApiName(api);
        m_stats.deviceName = "Unavailable";
        m_stats.available = false;
        m_stats.statusMessage = std::string(GraphicsApiName(api)) +
            " is planned for the GPU-scale roadmap but is not implemented in this build.";
    }

    bool Initialize(const RendererInitInfo&) override { return false; }
    void UploadAliveCells(std::span<const uint32_t>, glm::uvec3) override {}
    void RenderFrame(const RendererFrameInput&) override {}
    void Resize(uint32_t, uint32_t) override {}
    bool ReloadShaders() override { return false; }
    RendererStats Stats() const override { return m_stats; }
    void Shutdown() override {}

private:
    GraphicsApi m_api;
    RendererStats m_stats{};
};
}

const char* GraphicsApiName(GraphicsApi api)
{
    switch (api)
    {
    case GraphicsApi::Vulkan:
        return "Vulkan";
    case GraphicsApi::OpenGL:
        return "OpenGL";
    case GraphicsApi::D3D12:
        return "D3D12";
    case GraphicsApi::Metal:
        return "Metal";
    case GraphicsApi::WebGPU:
        return "WebGPU";
    }

    return "Unknown";
}

bool IsGraphicsApiSupported(GraphicsApi api)
{
    return api == GraphicsApi::OpenGL;
}

std::unique_ptr<IRenderer> CreateRenderer(GraphicsApi api)
{
    if (api == GraphicsApi::OpenGL)
    {
        return std::make_unique<OpenGLRenderer>();
    }

    return std::make_unique<UnsupportedRenderer>(api);
}
