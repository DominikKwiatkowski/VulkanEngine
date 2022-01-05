#define GLFW_INCLUDE_VULKAN
#include <algorithm>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cstdlib>


#include "Core/App.hpp"

int main()
{
    VulkanEngine::App app{};
    try
    {
        app.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_SUCCESS;
    }

    return EXIT_SUCCESS;
}
