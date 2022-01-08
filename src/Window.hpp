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

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        bool ShouldClose()
        {
            return glfwWindowShouldClose(pWindow);
        }

        VkExtent2D getExtent()
        {
            return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
        }

        bool WasWindowResized()
        {
            return framebufferResized;
        }

        void ResetWindowResizedFlag()
        {
            framebufferResized = false;
        }

        /// <summary>
        /// Create window surface
        /// </summary>
        /// <param name="instance"> Instance on which surface will be</param>
        /// <param name="surface"> pointer to surface</param>
        void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

        GLFWwindow* GetGLFWWindow()
        {
            return pWindow;
        }

        glm::dvec2 GetMousePos()
        {
            return mousePos;
        }

        /// <summary>
        /// Move cursor to window center
        /// </summary>
        void CenterMousePos();

    private:
        // Base glfw callbacks
        static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
        static void OnMauseMoveCallback(GLFWwindow* window, double xpos, double ypos);
        static void OnFocusCallback(GLFWwindow* window, int focus);

        GLFWwindow* pWindow;
        std::string windowName;

        int width;
        int height;
        bool framebufferResized = false;
        bool focusedRegained = true;
        bool focused = true;
        glm::dvec2 mousePos = {};
        void InitWindow();
    };
}
