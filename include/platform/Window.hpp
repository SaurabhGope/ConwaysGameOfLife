#pragma once

#include <cstdint>
#include <glm/vec2.hpp>

struct GLFWwindow;

class Window
{
public:
    bool Create(uint32_t width, uint32_t height, const char* title);
    void Destroy();

    bool ShouldClose() const;
    void PollEvents();
    void SwapBuffers();
    void MakeContextCurrent();
    glm::uvec2 FramebufferSize() const;

    void* NativeHandle() const;

private:
    GLFWwindow* m_window = nullptr;
};
