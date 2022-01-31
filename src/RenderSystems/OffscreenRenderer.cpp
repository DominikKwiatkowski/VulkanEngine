#include "OffscreenRenderer.hpp"

#include <array>
#include <stdexcept>

#include "SwapChain.hpp"

namespace VulkanEngine
{
    OffscreenRenderer::OffscreenRenderer(Device& device, int width, int height, VkFormat depthFormat):
    device(device),width(width),height(height), depthFormat(depthFormat)
    {
        CreateRenderPass();
        CreateFramebuffer();
    }

    OffscreenRenderer::~OffscreenRenderer()
    {
        // Color attachment
        vkDestroyImageView(device.GetDevice(), colorAttachment.view, nullptr);
        vkDestroyImage(device.GetDevice(), colorAttachment.image, nullptr);
        vkFreeMemory(device.GetDevice(), colorAttachment.mem, nullptr);

        // Depth attachment
        vkDestroyImageView(device.GetDevice(), depthAttachment.view, nullptr);
        vkDestroyImage(device.GetDevice(), depthAttachment.image, nullptr);
        vkFreeMemory(device.GetDevice(), depthAttachment.mem, nullptr);

        vkDestroyFramebuffer(device.GetDevice(), framebuffer, nullptr);

        vkDestroyRenderPass(device.GetDevice(), renderPass, nullptr);
    }

    void OffscreenRenderer::CreateFramebuffer()
    {
        VkImageCreateInfo imageInfo;
        Image::DefaultImageCreateInfo(imageInfo, width, height, VK_FORMAT_R32_SFLOAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
        VkImageSubresourceRange subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT,0,1,0,1 };
        device.CreateImageWithInfo(imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, colorAttachment.image, colorAttachment.mem);
        device.TransitionImageLayout(colorAttachment.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, subresourceRange);
        device.CreateImageView(colorAttachment.image, VK_FORMAT_R32_SFLOAT, colorAttachment.view, subresourceRange);

        subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
        imageInfo.format = depthFormat;
        imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        device.CreateImageWithInfo(imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthAttachment.image, depthAttachment.mem);
        device.TransitionImageLayout(colorAttachment.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, subresourceRange);
        device.CreateImageView(depthAttachment.image, depthFormat, depthAttachment.view, subresourceRange);

        std::array<VkImageView, 2> attachments = { colorAttachment.view, depthAttachment.view };

        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = width;
        framebufferInfo.height = height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(
            device.GetDevice(),
            &framebufferInfo,
            nullptr,
            &framebuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }

    void OffscreenRenderer::CreateRenderPass()
    {
        VkAttachmentDescription colorAttachment = {};

        colorAttachment.format = VK_FORMAT_R32_SFLOAT;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        // Depth attachment
        VkAttachmentDescription depthAttachment = {};
        depthAttachment.format = depthFormat;
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };

        VkAttachmentReference colorReference = {};
        colorReference.attachment = 0;
        colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthReference = {};
        depthReference.attachment = 1;
        depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorReference;
        subpass.pDepthStencilAttachment = &depthReference;

        VkRenderPassCreateInfo renderPassCreateInfo = {};
        renderPassCreateInfo.attachmentCount = sizeof(attachments);
        renderPassCreateInfo.pAttachments = attachments.data();
        renderPassCreateInfo.subpassCount = 1;
        renderPassCreateInfo.pSubpasses = &subpass;

        if(vkCreateRenderPass(device.GetDevice(), &renderPassCreateInfo, nullptr, &renderPass))
        {
            throw std::runtime_error("failed to create render pass!");
        }
    }
}
