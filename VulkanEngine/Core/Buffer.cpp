#pragma once

#include "Buffer.hpp"

namespace VulkanEngine
{
    VkDeviceSize Buffer::getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment)
    {
        if (minOffsetAlignment > 0)
        {
            return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
        }
        return instanceSize;
    }

    Buffer::Buffer(
        Device& device,
        VkDeviceSize instanceSize,
        uint32_t instanceCount,
        VkBufferUsageFlags usageFlags,
        VkMemoryPropertyFlags memoryPropertyFlags,
        VkDeviceSize minOffsetAlignment)
        : device{device},
          instanceSize{instanceSize},
          instanceCount{instanceCount},
          usageFlags{usageFlags},
          memoryPropertyFlags{memoryPropertyFlags}
    {
        alignmentSize = getAlignment(instanceSize, minOffsetAlignment);
        bufferSize = alignmentSize * instanceCount;
        device.CreateBuffer(bufferSize, usageFlags, memoryPropertyFlags, buffer, memory);
    }

    Buffer::~Buffer()
    {
        Unmap();
        vkDestroyBuffer(device.GetDevice(), buffer, nullptr);
        vkFreeMemory(device.GetDevice(), memory, nullptr);
    }

    VkResult Buffer::Map(VkDeviceSize size, VkDeviceSize offset)
    {
        assert(buffer && memory);
        return vkMapMemory(device.GetDevice(), memory, offset, size, 0, &mapped);
    }

    void Buffer::Unmap()
    {
        if (mapped)
        {
            vkUnmapMemory(device.GetDevice(), memory);
            mapped = nullptr;
        }
    }

    void Buffer::WriteToBuffer(void* data, VkDeviceSize size, VkDeviceSize offset)
    {
        assert(mapped);

        if (size == VK_WHOLE_SIZE)
        {
            memcpy(mapped, data, bufferSize);
        }
        else
        {
            char* memOffset = (char*)mapped;
            memOffset += offset;
            memcpy(memOffset, data, size);
        }
    }


    VkResult Buffer::Flush(VkDeviceSize size, VkDeviceSize offset)
    {
        VkMappedMemoryRange mappedRange = {};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = memory;
        mappedRange.offset = offset;
        mappedRange.size = size;
        return vkFlushMappedMemoryRanges(device.GetDevice(), 1, &mappedRange);
    }


    VkResult Buffer::Invalidate(VkDeviceSize size, VkDeviceSize offset)
    {
        VkMappedMemoryRange mappedRange = {};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = memory;
        mappedRange.offset = offset;
        mappedRange.size = size;
        return vkInvalidateMappedMemoryRanges(device.GetDevice(), 1, &mappedRange);
    }

    VkDescriptorBufferInfo Buffer::DescriptorInfo(VkDeviceSize size, VkDeviceSize offset)
    {
        return VkDescriptorBufferInfo{
            buffer,
            offset,
            size,
        };
    }

    void Buffer::WriteToIndex(void* data, int index)
    {
        WriteToBuffer(data, instanceSize, index * alignmentSize);
    }


    VkResult Buffer::FlushIndex(int index)
    {
        return Flush(alignmentSize, index * alignmentSize);
    }


    VkDescriptorBufferInfo Buffer::DescriptorInfoForIndex(int index)
    {
        return DescriptorInfo(alignmentSize, index * alignmentSize);
    }

    VkResult Buffer::InvalidateIndex(int index)
    {
        return Invalidate(alignmentSize, index * alignmentSize);
    }
}
