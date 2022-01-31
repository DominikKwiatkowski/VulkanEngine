#pragma once
#include "OffscreenShadowRenderSystem.hpp"
namespace VulkanEngine
{
    OffscreenShadowRenderSystem::OffscreenShadowRenderSystem(Device& device, VkRenderPass renderPass,
        std::vector<VkDescriptorSetLayout> descriptorSetLayouts)
            :RenderSystem(device)
    {
    }
    void OffscreenShadowRenderSystem::Render(FrameInfo frameInfo)
    {
    }

    void OffscreenShadowRenderSystem::CreatePipelineLayout(std::vector<VkDescriptorSetLayout> descriptorSetLayouts)
    {
    }

    void OffscreenShadowRenderSystem::CreatePipeline(VkRenderPass renderPass)
    {
    }
}
