#pragma once
#include <memory>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "Window.hpp"
#include "GameObject.hpp"
#include "Renderer.hpp"
#include "Camera.hpp"
#include "KeyboardController.hpp"
#include "Descriptors.hpp"

namespace VulkanEngine
{
    /// <summary>
    /// Main class of application, which is supposed to combine everything.
    /// </summary>
    class App
    {
    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

        /// <summary>
        /// App constructor will do vulkan, glfw and objects setup
        /// </summary>
        App();
        ~App();

        /// <summary>
        /// Main appliaction function. It will render frames
        /// until close command will be sent to window
        /// </summary>
        void run();

        App(const App&) = delete;
        App& operator=(const App&) = delete;

    private:
        Window window{WIDTH, HEIGHT, "VULKAN"};
        Device device{window};
        Renderer renderer{window, device};

        std::unique_ptr<DescriptorPool> globalPool{};
        GameObject::Map gameObjects;

        /// <summary>
        /// Load all game objects.
        /// TODO: we should have object pool per render system,
        ///     probably it is good idea to create some struct,
        ///     which combine this 2 things
        /// </summary>
        void LoadGameObjects();
    };
}
