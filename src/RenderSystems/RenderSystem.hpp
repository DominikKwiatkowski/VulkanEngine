#pragma once
#include <memory>

#include "Device.hpp"
#include "FrameInfo.hpp"
#include "Pipeline.hpp"

namespace VulkanEngine
{
    /// <summary>
    /// Abstract class to represent render systems.
    /// </summary>
    class RenderSystem
    {
    public:
        RenderSystem(Device& device):
            device(device)
        {
        }

        virtual ~RenderSystem()
        {
            vkDestroyPipelineLayout(device.GetDevice(), pipelineLayout, nullptr);
        }

        // Delete copy constructor
        RenderSystem(const RenderSystem&) = delete;
        RenderSystem& operator=(const RenderSystem&) = delete;

        /// <summary>
        /// Render objects. Each render system have their own rules what it will render.
        /// </summary>
        /// <param name="frameInfo"> Information about current frame</param>
        virtual void Render(FrameInfo frameInfo) = 0;
    protected:
        Device& device;
        std::unique_ptr<Pipeline> pipeline;
        VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    };
}
