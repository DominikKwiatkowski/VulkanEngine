#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "Image.hpp"

#include <stdexcept>

#include "Buffer.hpp"
#include "Device.hpp"

namespace VulkanEngine
{
    Image::Image(Device& device, VkImageCreateInfo imageInfo, VkMemoryPropertyFlagBits memoryProperties)
        : device(device)
    {
        device.CreateImageWithInfo(imageInfo, memoryProperties, image, imageMemory);
        device.CreateImageView(image, imageInfo.format, imageView);
        VkSamplerCreateInfo samplerInfo = {};
        DefaultSamplerCreateInfo(samplerInfo, device);
        if (vkCreateSampler(device.GetDevice(), &samplerInfo, nullptr, &imageSampler) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create texture sampler!");
        }
    }

    Image::~Image()
    {
        vkDestroySampler(device.GetDevice(), imageSampler, nullptr);
        vkDestroyImageView(device.GetDevice(), imageView, nullptr);
        vkDestroyImage(device.GetDevice(), image, nullptr);
        vkFreeMemory(device.GetDevice(), imageMemory, nullptr);
    }

    std::unique_ptr<Image> Image::LoadImageFromFile(const std::string& filepath, Device& device)
    {
        int width, height, texChannels;
        stbi_uc* pixels = stbi_load(filepath.c_str(), &width, &height, &texChannels, STBI_rgb_alpha);
        VkDeviceSize imageSize = width * height * 4;

        if (!pixels)
        {
            throw std::runtime_error("failed to load texture image!");
        }

        Buffer stagingBuffer(
            device,
            imageSize,
            1,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        stagingBuffer.Map();
        stagingBuffer.WriteToBuffer(pixels, imageSize);
        stbi_image_free(pixels);

        VkImageCreateInfo imageInfo = {};
        DefaultImageCreateInfo(imageInfo, width, height);
        auto image = std::make_unique<Image>(device, imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        device.TransitionImageLayout(image->GetImage(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        device.CopyBufferToImage(stagingBuffer.GetBuffer(), image->GetImage(), width, height, 1);
        device.TransitionImageLayout(image->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                     VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        return image;
    }

    void Image::DefaultImageCreateInfo(VkImageCreateInfo& imageInfo, int imageWidth, int imageHeight)
    {
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = static_cast<uint32_t>(imageWidth);
        imageInfo.extent.height = static_cast<uint32_t>(imageHeight);
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.flags = 0; // Optional
    }

    void Image::DefaultSamplerCreateInfo(VkSamplerCreateInfo& samplerInfo, Device& device)
    {
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = device.properties.limits.maxSamplerAnisotropy;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 0.0f;
    }

    VkDescriptorImageInfo Image::GetDescriptorInfo(VkImageLayout currentLayout)
    {
        return VkDescriptorImageInfo{ imageSampler,imageView, currentLayout };
    }
}
