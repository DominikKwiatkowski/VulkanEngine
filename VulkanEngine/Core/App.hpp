#pragma once
#include <memory>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "Window.hpp"
#include "GameObject.hpp"
#include "Renderer.hpp"

namespace VulkanEngine
{
    class App
    {
    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;
        App();
        ~App();
        void run();
        App(const App&) = delete;
        App &operator=(const App&) = delete;

    private:
        Window window{WIDTH, HEIGHT, "VULKAN"};
        Device device{ window };
        Renderer renderer{ window, device };
        std::vector<GameObject> gameObjects;
        
        void LoadGameObjects();
    };
}
