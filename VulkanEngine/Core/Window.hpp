#pragma once
#define GLFW_INCLUDE_VULKAN
#include <string>
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

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

        glm::dvec2 GetMousePos()
        {
            return mousePos;
        }

        void CenterMousePos();
    private:
        static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
        static void OnMauseMoveCallback(GLFWwindow* window, double xpos, double ypos);
        static void OnFocusCallback(GLFWwindow* window, int focus);
        GLFWwindow *pWindow;
        std::string windowName;

        int width;
        int height;
        bool framebufferResized = false;
        bool focusedRegeined = true;
        bool focused = true;
        glm::dvec2 mousePos = {};
        void InitWindow();
    };
}
