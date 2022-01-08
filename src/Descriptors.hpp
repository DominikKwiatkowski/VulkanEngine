#pragma once
#include <memory>
#include <unordered_map>

#include "Device.hpp"


namespace VulkanEngine
{
    class DescriptorSetLayout
    {
    public:

        class Builder
        {
        public:
            Builder(Device& device) : device(device) {}

            /// <summary>
            /// Add binding point to descriptor layout.
            /// </summary>
            /// <param name="binding"> Binding index</param>
            /// <param name="descriptorType"> VkDescriptorType in this binding</param>
            /// <param name="stageFlags"> VkShaderStageFlags to set shaders, which use it.</param>
            /// <param name="count"> Number of descriptors, one by default</param>
            /// <returns> Builder reference</returns>
            Builder& AddBinding(
                uint32_t binding,
                VkDescriptorType descriptorType,
                VkShaderStageFlags stageFlags,
                uint32_t count = 1);

            /// <summary>
            /// Function to build DescriptorSetLayout
            /// </summary>
            /// <returns> unique_ptr to DescriptorSetLayout build from given parameters</returns>
            std::unique_ptr<DescriptorSetLayout> Build() const;

        private:
            Device& device;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
        };

        /// <summary>
        /// Creates VkDescriptorSetLayout from given parameters.
        /// </summary>
        /// <param name="device"> Current device</param>
        /// <param name="bindings"> List of binding for this layout</param>
        DescriptorSetLayout(
            Device& device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);

        ~DescriptorSetLayout();

        // Disable copy to prevent double free.
        DescriptorSetLayout(const DescriptorSetLayout&) = delete;
        DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;

        VkDescriptorSetLayout GetDescriptorSetLayout() const { return descriptorSetLayout; }

    private:
        Device& device;
        VkDescriptorSetLayout descriptorSetLayout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

        friend class DescriptorWriter;
    };


    class DescriptorPool
    {
    public:

        class Builder
        {
        public:
            Builder(Device& device) : device{ device } {}

            /// <summary>
            ///
            /// </summary>
            /// <param name="descriptorType"> Type of descriptor in this pool</param>
            /// <param name="count"> Count of descriptors</param>
            /// <returns> Builder reference</returns>
            Builder& AddPoolSize(VkDescriptorType descriptorType, uint32_t count);

            /// <summary>
            /// Set flags of building pool.
            /// </summary>
            /// <param name="flags"> VkDescriptorPoolCreateFlags to be used in this pool</param>
            /// <returns> Builder reference</returns>
            Builder& SetPoolFlags(VkDescriptorPoolCreateFlags flags);

            /// <summary>
            /// Set maximum number of descriptors that can be allocated from pool
            /// </summary>
            /// <param name="count"> Maximum sets to be allocated</param>
            /// <returns> Builder reference</returns>
            Builder& SetMaxSets(uint32_t count);

            /// <summary>
            /// Build Descriptor pool from given parameters.
            /// </summary>
            /// <returns>unique_ptr to created descriptor pool from builder parameters.</returns>
            std::unique_ptr<DescriptorPool> Build() const;

        private:
            Device& device;
            std::vector<VkDescriptorPoolSize> poolSizes{};
            uint32_t maxSets = 1000;
            VkDescriptorPoolCreateFlags poolFlags = 0;
        };

        /// <summary>
        /// Create VkDescriptorPool.
        /// </summary>
        /// <param name="device"> Current device</param>
        /// <param name="maxSets"> Max number of sets, that can be allocated</param>
        /// <param name="poolFlags"> VkDescriptorPoolCreateFlags to be used</param>
        /// <param name="poolSizes"> List of pool sizes to be used in this pool</param>
        DescriptorPool(
            Device& device,
            uint32_t maxSets,
            VkDescriptorPoolCreateFlags poolFlags,
            const std::vector<VkDescriptorPoolSize>& poolSizes);

        ~DescriptorPool();

        // Remove copy to prevent double free.
        DescriptorPool(const DescriptorPool&) = delete;
        DescriptorPool& operator=(const DescriptorPool&) = delete;

        /// <summary>
        /// Allocate descriptor in pool
        /// </summary>
        /// <param name="descriptorSetLayout"> Layout of descriptor set</param>
        /// <param name="descriptor">Descriptor to be allocated</param>
        /// <returns>True if success, false otherwise</returns>
        bool AllocateDescriptor(
            VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

        /// <summary>
        /// Free memory of given descriptor
        /// </summary>
        /// <param name="descriptors">Descriptor to be free</param>
        void FreeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

        /// <summary>
        /// Perform vkResetDescriptorPool on pool
        /// </summary>
        void ResetPool();

    private:
        Device& device;
        VkDescriptorPool descriptorPool;
        friend class DescriptorWriter;
    };

    class DescriptorWriter
    {
    public:
        DescriptorWriter(DescriptorSetLayout& setLayout, DescriptorPool& pool);

        /// <summary>
        /// Add buffer to be write in given binding.
        /// </summary>
        /// <param name="binding"> Binding position</param>
        /// <param name="pBufferInfo"> VkDescriptorBufferInfo* struct about this buffer</param>
        /// <returns> Reference to this obj</returns>
        DescriptorWriter& WriteBuffer(uint32_t binding, VkDescriptorBufferInfo* pBufferInfo);

        /// <summary>
        /// Add image to be write in given binding.
        /// </summary>
        /// <param name="binding"> Binding position</param>
        /// <param name="pImageInfo"> VkDescriptorImageInfo* struct about this image</param>
        /// <returns> Reference to this obj</returns>
        DescriptorWriter& WriteImage(uint32_t binding, VkDescriptorImageInfo* pImageInfo);

        /// <summary>
        /// Allocate and overwrite Descriptors to given set
        /// </summary>
        /// <param name="set"> DescriptorSet to build.\</param>
        /// <returns> True if success, false otherwise</returns>
        bool Build(VkDescriptorSet& set);

        /// <summary>
        /// Overwrite set by builder params.
        /// </summary>
        /// <param name="set"></param>
        void Overwrite(VkDescriptorSet& set);

    private:
        DescriptorSetLayout& setLayout;
        DescriptorPool& pool;
        std::vector<VkWriteDescriptorSet> writes;
    };

}
