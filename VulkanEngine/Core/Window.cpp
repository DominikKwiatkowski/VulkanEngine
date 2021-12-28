#pragma once
#include "Window.hpp"

#include <stdexcept>

namespace VulkanEngine
{
    Window::Window(int width, int height, std::string name)
        :width(width), height(height),windowName(name)
    {
        InitWindow();
    }

    void Window::InitWindow()
    {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        pWindow = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(pWindow, this);
        glfwSetFramebufferSizeCallback(pWindow, FrambufferResizeCallback);
    }

    Window::~Window()
    {
        glfwDestroyWindow(pWindow);

        glfwTerminate();
    }

    void Window::CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
    {
        if (glfwCreateWindowSurface(instance, pWindow, nullptr, surface) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create window surface");
        }
    }

    void Window::FrambufferResizeCallback(GLFWwindow* window, int width, int height)
    {
        auto pWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        pWindow->framebufferResized = true;
        pWindow->width = width;
        pWindow->height = height;
    }

}
