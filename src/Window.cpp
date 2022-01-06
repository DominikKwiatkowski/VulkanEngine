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
        glfwSetFramebufferSizeCallback(pWindow, FramebufferResizeCallback);
        glfwSetCursorPosCallback(pWindow, OnMauseMoveCallback);
        glfwSetWindowFocusCallback(pWindow, OnFocusCallback);
        glfwSetInputMode(pWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
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
        if(!pWindow->focusedRegained && pWindow->focused)
        {
            pWindow->mousePos.x = xpos;
            pWindow->mousePos.y = ypos;
        }
        else if(pWindow->focusedRegained)
        {
            pWindow->focusedRegained = false;
        }
    }

    void Window::OnFocusCallback(GLFWwindow* window, int focus)
    {
        auto pWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        if(focus)
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
