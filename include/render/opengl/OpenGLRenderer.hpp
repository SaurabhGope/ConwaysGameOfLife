#pragma once

#include "render/IRenderer.hpp"

class OpenGLRenderer final : public IRenderer
{
public:
    bool Initialize(const RendererInitInfo& initInfo) override;
    void UploadAliveCells(std::span<const uint32_t> aliveLinearIndices, glm::uvec3 dims) override;
    void RenderFrame(const RendererFrameInput& input) override;
    void Resize(uint32_t width, uint32_t height) override;
    bool ReloadShaders() override;
    RendererStats Stats() const override;
    void Shutdown() override;

private:
    RendererStats m_stats{};
    bool m_initialized = false;
};
