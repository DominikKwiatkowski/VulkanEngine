#pragma once
#include <memory>
#include <string>

#include "Device.hpp"


namespace VulkanEngine
{
    class Image
    {
    public:
        Image(Device& device, VkImageCreateInfo imageInfo, VkMemoryPropertyFlagBits memoryProperties);
        ~Image();
        Image(const Image&) = delete;
        Image& operator=(const Image&) = delete;

        static std::unique_ptr<Image> LoadImageFromFile(const std::string& filepath, Device& device);
        static void DefaultImageCreateInfo(VkImageCreateInfo& imageInfo, int imageWidth, int imageHeight);
        static void DefaultSamplerCreateInfo(VkSamplerCreateInfo& samplerInfo, Device& device);

        VkImage GetImage()
        {
            return image;
        }

        VkImageView GetImageView()
        {
            return imageView;
        }

        VkDescriptorImageInfo GetDescriptorInfo(VkImageLayout currentLayout);
    private:
        Device& device;
        VkImage image = VK_NULL_HANDLE;
        VkImageView imageView = VK_NULL_HANDLE;
        VkDeviceMemory imageMemory = VK_NULL_HANDLE;
        VkSampler imageSampler = VK_NULL_HANDLE;
    };
}
