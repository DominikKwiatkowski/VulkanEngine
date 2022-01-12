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

        static std::unique_ptr<Image> LoadImageFromFile(std::string& filepath, Device& device);
        static void DefaultImageCreateInfo(VkImageCreateInfo& imageInfo, int imageWidth, int imageHeight);


        VkImage GetImage()
        {
            return image;
        }
    private:
        Device& device;
        VkImage image = VK_NULL_HANDLE;
        VkDeviceMemory imageMemory = VK_NULL_HANDLE;
    };
}
