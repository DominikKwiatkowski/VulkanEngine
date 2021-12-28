#pragma once
#include <memory>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "Model.hpp"
#include "Pipeline.hpp"
#include "Window.hpp"
#include "SwapChain.hpp"
#include "GameObject.hpp"

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
        std::unique_ptr < SwapChain> swapChain;
        std::unique_ptr<Pipeline> pipeline;
        VkPipelineLayout pipelineLayout;
        std::vector<VkCommandBuffer> commandBuffers;
        std::vector<GameObject> gameObjects;

        void CreatePipelineLayout();
        void CreatePipeline();
        void CreateCommandBuffer();
        void FreeCommandBuffer();
        void DrawFrame();
        void LoadGameObjects();
        void Sierpinski(std::vector<Model::Vertex>& vertices,
            int depth,
            glm::vec2 left,
            glm::vec2 right,
            glm::vec2 top);

        void RecreateSwapChain();
        void RecordCommandBuffer(int imageIndex);
        void RenderGameObjects(VkCommandBuffer commandBuffer);
    };
}
