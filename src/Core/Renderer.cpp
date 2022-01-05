#pragma once
#include "Renderer.hpp"

#include <array>
#include <stdexcept>

namespace VulkanEngine
{
    Renderer::Renderer(Window &window, Device &device):
        window(window),
        device(device)
    {
        RecreateSwapChain();
        CreateCommandBuffer();
    }

    Renderer::~Renderer()
    {
        FreeCommandBuffer();
    }

    void Renderer::CreateCommandBuffer()
    {
        commandBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = device.GetCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if (vkAllocateCommandBuffers(device.GetDevice(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate command buffers");
        }
    }

    void Renderer::FreeCommandBuffer()
    {
        if (commandBuffers.size() > 0)
            vkFreeCommandBuffers(
                device.GetDevice(),
                device.GetCommandPool(),
                static_cast<uint32_t>(commandBuffers.size()),
                commandBuffers.data());
    }

    void Renderer::RecreateSwapChain()
    {
        auto extent = window.getExtent();
        while (extent.width == 0 || extent.height == 0)
        {
            extent = window.getExtent();
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(device.GetDevice());
        if (swapChain != nullptr)
        {
            std::shared_ptr<SwapChain> oldSwapChain = std::move(swapChain);
            swapChain = std::make_unique<SwapChain>(device, extent, oldSwapChain);

            if (!oldSwapChain->compareSwapFormat(*swapChain.get()))
            {
                throw std::runtime_error("Swap chain format has changed");
            }
        }
        else
        {
            swapChain = std::make_unique<SwapChain>(device, extent);
        }
        // CreatePipeline();
    }

    VkCommandBuffer Renderer::BeginFrame()
    {
        assert(!isFrameStarted);

        auto result = swapChain->AcquireNextImage(&currentImageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            RecreateSwapChain();
            return nullptr;
        }
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("failed to acquire next image");
        }

        isFrameStarted = true;
        auto commandBuffer = GetCurrentCommandBuffer();

        VkCommandBufferBeginInfo beginInfo{};

        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to start recording command buffer");
        }

        return commandBuffer;
    }

    void Renderer::EndFrame()
    {
        assert(isFrameStarted);
        auto commandBuffer = GetCurrentCommandBuffer();
        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to stop recording command buffer");
        }



        auto result = swapChain->SubmitCommandBuffers(&commandBuffer, &currentImageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.WasWindowResized())
        {
            window.ResetWindowResizedFlag();
            RecreateSwapChain();
        }

        isFrameStarted = false;
        currentFrameIndex = (currentFrameIndex + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
    }

    void Renderer::BeginSwapChainRenderPass(VkCommandBuffer commandBuffer)
    {
        assert(isFrameStarted);
        assert(commandBuffer == GetCurrentCommandBuffer());

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = swapChain->GetRenderPass();
        renderPassInfo.framebuffer = swapChain->GetFrameBuffer(currentImageIndex);

        renderPassInfo.renderArea.offset = { 0,0 };
        renderPassInfo.renderArea.extent = swapChain->GetSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = { 0.01f,0.01f,0.01f,1.0f };
        clearValues[1].depthStencil = { 1.0f,0 };
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(swapChain->GetSwapChainExtent().width);
        viewport.height = static_cast<float>(swapChain->GetSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{ {0, 0}, swapChain->GetSwapChainExtent() };
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }

    void Renderer::EndSwapChainRenderPass(VkCommandBuffer commandBuffer)
    {
        assert(isFrameStarted);
        assert(commandBuffer == GetCurrentCommandBuffer());

        vkCmdEndRenderPass(commandBuffer);
    }

}
