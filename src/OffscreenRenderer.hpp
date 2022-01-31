#pragma once
#include "Device.hpp"
#include "Image.hpp"

namespace VulkanEngine
{
    struct FrameBufferAttachment
    {
        VkImage image;
        VkDeviceMemory mem;
        VkImageView view;
    };
    class OffscreenRenderer
    {
    public:
        OffscreenRenderer(Device& device, int width, int height, VkFormat depthFormat);
        ~OffscreenRenderer();

        OffscreenRenderer(const OffscreenRenderer&) = delete;
        OffscreenRenderer& operator=(const OffscreenRenderer&) = delete;

        VkRenderPass GetRenderPass()
        {
            return renderPass;
        }

    private:
        void CreateRenderPass();
        void CreateFramebuffer();

        const int width;
        const int height;
        Device& device;
        VkRenderPass renderPass;
        FrameBufferAttachment colorAttachment;
        FrameBufferAttachment depthAttachment;
        VkFramebuffer framebuffer;
        VkFormat depthFormat;

    };
}
