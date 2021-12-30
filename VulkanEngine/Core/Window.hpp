#pragma once
#define GLFW_INCLUDE_VULKAN
#include <string>
#include <GLFW/glfw3.h>

namespace VulkanEngine
{
    class Window
    {
    public:
        Window(int width, int height, std::string name);
        ~Window();

        Window(const Window &) = delete;
        Window& operator=(const Window&) = delete;

        bool ShouldClose()
        {
            return glfwWindowShouldClose(pWindow);
        }
        VkExtent2D getExtent()
        {
            return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
        }
        bool WasWindowResized()
        {
            return framebufferResized;
        }
        void ResetWindowResizedFlag()
        {
            framebufferResized = false;
        }

        void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

        GLFWwindow* GetGLFWWindow()
        {
            return pWindow;
        }
    private:
        static void FrambufferResizeCallback(GLFWwindow* window, int width, int height);
        GLFWwindow *pWindow;
        std::string windowName;

        int width;
        int height;
        bool framebufferResized = false;

        void InitWindow();
    };
}
