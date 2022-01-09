#pragma once
#include "Device.hpp"


namespace VulkanEngine
{
    /// <summary>
    /// Class representing buffer. It wraps vulkan buffer methods and prevents memory leaks.
    /// </summary>
    class Buffer
    {
    public:
        /// <summary>
        /// Store data, calculate size and alignment, create buffer
        /// </summary>
        /// <param name="device"> Current device in program.</param>
        /// <param name="instanceSize"> Size of one instance inside buffer.</param>
        /// <param name="instanceCount"> Number of instance in buffer.</param>
        /// <param name="usageFlags"> Vk type of buffer usage, see VkBufferUsageFlagBits</param>
        /// <param name="memoryPropertyFlags"> Vk type of memory property, see VkMemoryPropertyFlagBits</param>
        /// <param name="minOffsetAlignment"> Ste offset alignment between each instance. </param>
        Buffer(
            Device& device,
            VkDeviceSize instanceSize,
            uint32_t instanceCount,
            VkBufferUsageFlags usageFlags,
            VkMemoryPropertyFlags memoryPropertyFlags,
            VkDeviceSize minOffsetAlignment = 1);

        ~Buffer();

        // Delete copy constructor and operator to avoid double free.
        Buffer(const Buffer&) = delete;
        Buffer& operator=(const Buffer&) = delete;

        /// <summary>
        /// Map buffer.
        /// </summary>
        /// <param name="size"> Size of memory to map. By default VK_WHOLE_SIZE</param>
        /// <param name="offset"> Offset of mapping memory.</param>
        /// <returns> vkMapMemory result. </returns>
        VkResult Map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

        /// <summary>
        /// Unmap buffer.
        /// </summary>
        void Unmap();

        /// <summary>
        /// Write to buffer on host. Note that buffer should NOT be mapped.
        /// </summary>
        /// <param name="data"> Data to write.</param>
        /// <param name="size"> Size to write, by default VK_WHOLE_SIZE.</param>
        /// <param name="offset"> Offset of buffer, 0 by default.</param>
        void WriteToBuffer(void* data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

        /// <summary>
        /// Flush i.e. copy buffer from host to device
        /// </summary>
        /// <param name="size"> Size to be flushed. VK_WHOLE_SIZE by default.</param>
        /// <param name="offset"> Offset from beginning of buffer. 0 by default.</param>
        /// <returns> Result of vkFlushMappedMemoryRanges.</returns>
        VkResult Flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

        /// <summary>
        /// Get descriptor of buffer
        /// </summary>
        /// <param name="size"> Size of interest. VK_WHOLE_SIZE by default.</param>
        /// <param name="offset"> Offset from beginning of buffer. 0 by default.</param>
        /// <returns> result of VkDescriptorBufferInfo.</returns>
        VkDescriptorBufferInfo DescriptorInfo(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

        /// <summary>
        /// Invalidates memory.
        /// </summary>
        /// <param name="size"> Size to invalidate. VK_WHOLE_SIZE by default.</param>
        /// <param name="offset"> Offset from beginning of buffer. 0 by default.</param>
        /// <returns> Result of vkInvalidateMappedMemoryRanges.</returns>
        VkResult Invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

        // This methods are similar, but works on only one, given instance

        /// <summary>
        /// Write data to host buffer memory
        /// </summary>
        /// <param name="data"> Data to be write.</param>
        /// <param name="index"> Index of instance.</param>
        void WriteToIndex(void* data, int index);

        /// <summary>
        /// Flush data in given index.
        /// </summary>
        /// <param name="index"> Index of instance.</param>
        /// <returns> Result of vkFlushMappedMemoryRanges.</returns>
        VkResult FlushIndex(int index);

        /// <summary>
        /// Get descriptor info of given index.
        /// </summary>
        /// <param name="index"> Index of instance.</param>
        /// <returns> Result of VkDescriptorBufferInfo.</returns>
        VkDescriptorBufferInfo DescriptorInfoForIndex(int index);

        /// <summary>
        /// Invalidate memory in given index.
        /// </summary>
        /// <param name="index"> Index of instance.</param>
        /// <returns> Result of vkInvalidateMappedMemoryRanges.</returns>
        VkResult InvalidateIndex(int index);

        // Getters of private members.

        VkBuffer GetBuffer() const
        {
            return buffer;
        }

        void* GetMappedMemory() const
        {
            return mapped;
        }

        uint32_t GetInstanceCount() const
        {
            return instanceCount;
        }

        VkDeviceSize GetInstanceSize() const
        {
            return instanceSize;
        }

        VkDeviceSize GetAlignmentSize() const
        {
            return alignmentSize;
        }

        VkBufferUsageFlags GetUsageFlags() const
        {
            return usageFlags;
        }

        VkMemoryPropertyFlags GetMemoryPropertyFlags() const
        {
            return memoryPropertyFlags;
        }

        VkDeviceSize GetBufferSize() const
        {
            return bufferSize;
        }

    private:
        /// <summary>
        /// Calculate alignment for given minOffsetAlignment and instance size
        /// </summary>
        /// <param name="instanceSize"> Size of instance.</param>
        /// <param name="minOffsetAlignment"> Minimum offset alignment, specified by device.</param>
        /// <returns> Alignment for given data.</returns>
        static VkDeviceSize getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment);

        Device& device;
        void* mapped = nullptr;
        VkBuffer buffer = VK_NULL_HANDLE;
        VkDeviceMemory memory = VK_NULL_HANDLE;

        VkDeviceSize bufferSize;
        uint32_t instanceCount;
        VkDeviceSize instanceSize;
        VkDeviceSize alignmentSize;
        VkBufferUsageFlags usageFlags;
        VkMemoryPropertyFlags memoryPropertyFlags;
    };
}
