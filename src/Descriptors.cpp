#pragma once
#include "Descriptors.hpp"

#include <stdexcept>


namespace VulkanEngine
{
    DescriptorSetLayout::Builder& DescriptorSetLayout::Builder::AddBinding(
        uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, uint32_t count)
    {
        // Check if binding exist.
        assert(bindings.count(binding) == 0);

        VkDescriptorSetLayoutBinding layoutBinding{};
        layoutBinding.binding = binding;
        layoutBinding.descriptorType = descriptorType;
        layoutBinding.descriptorCount = count;
        layoutBinding.stageFlags = stageFlags;
        bindings[binding] = layoutBinding;

        return *this;
    }

    std::unique_ptr<DescriptorSetLayout> DescriptorSetLayout::Builder::Build() const
    {
        return std::make_unique<DescriptorSetLayout>(device, bindings);
    }

    DescriptorSetLayout::DescriptorSetLayout(
        Device& device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings):
        device(device), bindings(bindings)
    {
        std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
        for (auto kv : bindings)
        {
            setLayoutBindings.push_back(kv.second);
        }

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
        descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
        descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

        if (vkCreateDescriptorSetLayout(
            device.GetDevice(),
            &descriptorSetLayoutInfo,
            nullptr,
            &descriptorSetLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    DescriptorSetLayout::~DescriptorSetLayout()
    {
        vkDestroyDescriptorSetLayout(device.GetDevice(), descriptorSetLayout, nullptr);
    }

    DescriptorPool::Builder& DescriptorPool::Builder::AddPoolSize(VkDescriptorType descriptorType, uint32_t count)
    {
        VkDescriptorPoolSize poolSize{};
        poolSize.type = descriptorType;
        poolSize.descriptorCount = count;
        poolSizes.push_back(poolSize);

        return *this;
    }

    DescriptorPool::Builder& DescriptorPool::Builder::SetPoolFlags(VkDescriptorPoolCreateFlags flags)
    {
        poolFlags = flags;
        return *this;
    }

    DescriptorPool::Builder& DescriptorPool::Builder::SetMaxSets(uint32_t count)
    {
        maxSets = count;
        return *this;
    }

    std::unique_ptr<DescriptorPool> DescriptorPool::Builder::Build() const
    {
        return std::make_unique<DescriptorPool>(device, maxSets, poolFlags, poolSizes);
    }

    DescriptorPool::DescriptorPool(Device& device, uint32_t maxSets, VkDescriptorPoolCreateFlags poolFlags,
                                   const std::vector<VkDescriptorPoolSize>& poolSizes):
        device(device)
    {
        VkDescriptorPoolCreateInfo descriptorPoolInfo{};
        descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        descriptorPoolInfo.pPoolSizes = poolSizes.data();
        descriptorPoolInfo.maxSets = maxSets;
        descriptorPoolInfo.flags = poolFlags;

        if (vkCreateDescriptorPool(device.GetDevice(), &descriptorPoolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    DescriptorPool::~DescriptorPool()
    {
        vkDestroyDescriptorPool(device.GetDevice(), descriptorPool, nullptr);
    }

    bool DescriptorPool::AllocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout,
                                            VkDescriptorSet& descriptor) const
    {
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &descriptorSetLayout;

        if (vkAllocateDescriptorSets(device.GetDevice(), &allocInfo, &descriptor) != VK_SUCCESS)
        {
            return false;
        }
        return true;
    }

    void DescriptorPool::FreeDescriptors(std::vector<VkDescriptorSet>& descriptors) const
    {
        vkFreeDescriptorSets(
            device.GetDevice(),
            descriptorPool,
            static_cast<uint32_t>(descriptors.size()),
            descriptors.data());
    }

    void DescriptorPool::ResetPool()
    {
        vkResetDescriptorPool(device.GetDevice(), descriptorPool, 0);
    }

    DescriptorWriter::DescriptorWriter(DescriptorSetLayout& setLayout, DescriptorPool& pool):
        setLayout(setLayout), pool(pool)
    {
    }

    DescriptorWriter& DescriptorWriter::WriteBuffer(uint32_t binding, VkDescriptorBufferInfo* pBufferInfo)
    {
        assert(setLayout.bindings.count(binding) == 1);

        const auto& bindingDescription = setLayout.bindings[binding];

        assert(bindingDescription.descriptorCount == 1);

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = binding;
        write.pBufferInfo = pBufferInfo;
        write.descriptorCount = 1;

        writes.push_back(write);
        return *this;
    }

    DescriptorWriter& DescriptorWriter::WriteImage(uint32_t binding, VkDescriptorImageInfo* pImageInfo)
    {
        assert(setLayout.bindings.count(binding) == 1);

        auto& bindingDescription = setLayout.bindings[binding];

        assert(bindingDescription.descriptorCount == 1);

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = binding;
        write.pImageInfo = pImageInfo;
        write.descriptorCount = 1;

        writes.push_back(write);
        return *this;
    }

    bool DescriptorWriter::Build(VkDescriptorSet& set)
    {
        bool success = pool.AllocateDescriptor(setLayout.GetDescriptorSetLayout(), set);
        if (!success)
        {
            return false;
        }
        Overwrite(set);
        return true;
    }

    void DescriptorWriter::Overwrite(VkDescriptorSet& set)
    {
        for (auto& write : writes)
        {
            write.dstSet = set;
        }
        vkUpdateDescriptorSets(pool.device.GetDevice(), writes.size(), writes.data(), 0, nullptr);
    }
}
