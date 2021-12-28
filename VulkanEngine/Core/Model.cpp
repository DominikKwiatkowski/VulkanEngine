#pragma once
#include "Model.hpp"

#include <stdexcept>

namespace VulkanEngine
{
    Model::Model(Device& device, const std::vector<Vertex>& vertices):
        device{device}
    {
        CreateVertexBuffer(vertices);
    }

    Model::~Model()
    {
        vkDestroyBuffer(device.GetDevice(), vertexBuffer, nullptr);
        vkFreeMemory(device.GetDevice(), vertexBufferMemory, nullptr);
    }

    void Model::CreateVertexBuffer(const std::vector<Vertex>& vertices)
    {
        vertexCount = static_cast<uint32_t>(vertices.size());
        assert(vertexCount >= 3);
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
        device.CreateBuffer(
            bufferSize,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            vertexBuffer,
            vertexBufferMemory);

        void* data;
        if(vkMapMemory(device.GetDevice(), vertexBufferMemory, 0, bufferSize, 0, &data) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to map memory");
        }
        memcpy(data, vertices.data(), bufferSize);
        vkUnmapMemory(device.GetDevice(), vertexBufferMemory);
    }

    void Model::Bind(VkCommandBuffer commandBuffer)
    {
        VkBuffer buffers[] = { vertexBuffer };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
    }

    void Model::Draw(VkCommandBuffer commandBuffer)
    {
        vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
    }

    std::vector<VkVertexInputBindingDescription> Model::Vertex::getBindingDescription()
    {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(Vertex);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescriptions;
    }

    std::vector<VkVertexInputAttributeDescription> Model::Vertex::getAttributeDescriptions()
    {
        std::vector<VkVertexInputAttributeDescription> attribute(2);
        attribute[0].binding = 0;
        attribute[0].offset = offsetof(Vertex, position);
        attribute[0].format = VK_FORMAT_R32G32_SFLOAT;
        attribute[0].location = 0;

        attribute[1].binding = 0;
        attribute[1].offset = offsetof(Vertex,color);
        attribute[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attribute[1].location = 1;

        return attribute;
    }

}
