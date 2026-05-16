#include "platform/Window.hpp"

#include <GLFW/glfw3.h>

bool Window::Create(uint32_t width, uint32_t height, const char* title)
{
    m_window = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), title, nullptr, nullptr);
    return m_window != nullptr;
}

void Window::Destroy()
{
    if (m_window != nullptr)
    {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
}

bool Window::ShouldClose() const
{
    return m_window == nullptr || glfwWindowShouldClose(m_window) != 0;
}

void Window::PollEvents()
{
    glfwPollEvents();
}

void Window::SwapBuffers()
{
    if (m_window != nullptr)
    {
        glfwSwapBuffers(m_window);
    }
}

void Window::MakeContextCurrent()
{
    if (m_window != nullptr)
    {
        glfwMakeContextCurrent(m_window);
    }
}

glm::uvec2 Window::FramebufferSize() const
{
    int width = 0;
    int height = 0;
    if (m_window != nullptr)
    {
        glfwGetFramebufferSize(m_window, &width, &height);
    }

    return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
}

void* Window::NativeHandle() const
{
    return m_window;
}
