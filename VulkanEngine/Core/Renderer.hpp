#pragma once
#include <memory>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "Model.hpp"
#include "Window.hpp"
#include "SwapChain.hpp"

namespace VulkanEngine
{
    class Renderer
    {
    public:
        Renderer(Window &window, Device &device);
        ~Renderer();
        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;

        bool isFrameInProgress() const
        {
            return isFrameStarted;
        }

        VkCommandBuffer GetCurrentCommandBuffer() const
        {
            // can't get command buffer when frame not started
            assert(isFrameStarted);
            return commandBuffers[currentFrameIndex];
        }

        VkRenderPass getSwapChainRenderPass() const
        {
            return swapChain->GetRenderPass();
        }

        int GetFrameIndex() const
        {
            assert(isFrameStarted);
            return currentFrameIndex;
        }

        VkCommandBuffer BeginFrame();
        void EndFrame();
        void BeginSwapChainRenderPass(VkCommandBuffer commandBuffer);
        void EndSwapChainRenderPass(VkCommandBuffer commandBuffer);

    private:
        void CreateCommandBuffer();
        void FreeCommandBuffer();
        void RecreateSwapChain();

        Window& window;
        Device& device;
        std::unique_ptr < SwapChain> swapChain;
        std::vector<VkCommandBuffer> commandBuffers;

        uint32_t currentImageIndex;
        int currentFrameIndex;
        bool isFrameStarted = false;
    };
}
