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
    /// <summary>
    /// Object to store and menage all render data such as command buffer, swap chain and render pass
    /// </summary>
    class Renderer
    {
    public:
        /// <summary>
        /// Create swap chain and command buffer pool for new renderer
        /// </summary>
        /// <param name="window"> current window</param>
        /// <param name="device"> current device</param>
        Renderer(Window& window, Device& device);
        ~Renderer();
        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;

        float GetAspectRatio() const
        {
            return swapChain->ExtentAspectRatio();
        }

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

        /// <summary>
        /// Start new frame.
        /// </summary>
        /// <returns> New frame command buffer</returns>
        VkCommandBuffer BeginFrame();

        /// <summary>
        /// End current frame.
        /// </summary>
        void EndFrame();

        /// <summary>
        /// Begin render pass for given command buffer
        /// </summary>
        /// <param name="commandBuffer"> Current command buffer</param>
        void BeginSwapChainRenderPass(VkCommandBuffer commandBuffer);

        /// <summary>
        /// End swap chain for current render pass
        /// </summary>
        /// <param name="commandBuffer"> Current command buffer</param>
        void EndSwapChainRenderPass(VkCommandBuffer commandBuffer);

    private:
        /// <summary>
        /// Create new command buffer pool
        /// </summary>
        void CreateCommandBuffers();

        /// <summary>
        /// Delete current command buffer pool
        /// </summary>
        void FreeCommandBuffers();

        /// <summary>
        /// Recreate swap chain. Needed when resized.
        /// </summary>
        void RecreateSwapChain();

        Window& window;
        Device& device;
        std::unique_ptr<SwapChain> swapChain;
        std::vector<VkCommandBuffer> commandBuffers;

        uint32_t currentImageIndex = 0;
        int currentFrameIndex = 0;
        bool isFrameStarted = false;
    };
}
