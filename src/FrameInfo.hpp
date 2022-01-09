#pragma once
#include "Camera.hpp"

#include <vulkan/vulkan.h>

namespace VulkanEngine
{
    /// <summary>
    /// Struct to wrap all information about frame.
    /// </summary>
    struct FrameInfo
    {
        int frameIndex;
        float frameTime;
        Camera& camera;
        VkCommandBuffer commandBuffer;
        VkDescriptorSet globalDescriptorSet;
        GameObject::Map& gameObjects;
    };
}
