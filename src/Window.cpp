#pragma once
#include "Window.hpp"

#include <stdexcept>
#include <utility>

namespace VulkanEngine
{
    Window::Window(int width, int height, std::string name)
        : windowName(std::move(name)), width(width), height(height)
    {
        InitWindow();
    }

    void Window::InitWindow()
    {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        pWindow = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);

        // Set user pointer to access this data inside callbacks
        glfwSetWindowUserPointer(pWindow, this);

        // Set callbacks
        glfwSetFramebufferSizeCallback(pWindow, FramebufferResizeCallback);
        glfwSetCursorPosCallback(pWindow, OnMauseMoveCallback);
        glfwSetWindowFocusCallback(pWindow, OnFocusCallback);
        glfwSetInputMode(pWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

        // Fix start mouse pos
        CenterMousePos();
        mousePos.x = width / 2.0;
        mousePos.y = height / 2.0;
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

    void Window::CenterMousePos()
    {
        glfwSetCursorPos(pWindow, width / 2.0, height / 2.0);
        mousePos.x = width / 2;
        mousePos.y = height / 2;
    }

    void Window::FramebufferResizeCallback(GLFWwindow* window, int width, int height)
    {
        auto pWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        pWindow->framebufferResized = true;
        pWindow->width = width;
        pWindow->height = height;
    }

    void Window::OnMauseMoveCallback(GLFWwindow* window, double xpos, double ypos)
    {
        auto pWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        if (!pWindow->focusedRegained && pWindow->focused)
        {
            pWindow->mousePos.x = xpos;
            pWindow->mousePos.y = ypos;
        }
        else if (pWindow->focusedRegained)
        {
            pWindow->focusedRegained = false;
        }
    }

    void Window::OnFocusCallback(GLFWwindow* window, int focus)
    {
        auto pWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        if (focus)
        {
            pWindow->focusedRegained = true;
            pWindow->focused = true;
        }
        else
        {
            pWindow->focused = false;
        }
    }
}
