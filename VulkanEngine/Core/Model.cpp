#pragma once
#include "Model.hpp"
#include "Utils.hpp"
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#define  GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <stdexcept>
#include <unordered_map>

namespace std
{
    template<>
    struct hash<VulkanEngine::Model::Vertex>
    {
        size_t operator()(VulkanEngine::Model::Vertex const &vertex) const
        {
            size_t seed;
            VulkanEngine::HashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.texCord);
            return seed;
        }
    };
}

namespace VulkanEngine
{
    Model::Model(Device& device, const ModelData& builder):
        device{device}
    {
        CreateVertexBuffer(builder.vertices);
        CreateIndexBuffer(builder.indices);
    }

    Model::~Model()
    {
        vkDestroyBuffer(device.GetDevice(), vertexBuffer, nullptr);
        vkFreeMemory(device.GetDevice(), vertexBufferMemory, nullptr);

        if(hasIndexBuffer)
        {
            vkDestroyBuffer(device.GetDevice(), indexBuffer, nullptr);
            vkFreeMemory(device.GetDevice(), indexBufferMemory, nullptr);
        }
    }

    void Model::CreateVertexBuffer(const std::vector<Vertex>& vertices)
    {
        vertexCount = static_cast<uint32_t>(vertices.size());
        assert(vertexCount >= 3);

        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        device.CreateBuffer(
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer,
            stagingBufferMemory);

        void* data;
        if (vkMapMemory(device.GetDevice(), stagingBufferMemory, 0, bufferSize, 0, &data) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to map memory");
        }
        memcpy(data, vertices.data(), bufferSize);
        vkUnmapMemory(device.GetDevice(), stagingBufferMemory);


        device.CreateBuffer(
            bufferSize,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            vertexBuffer,
            vertexBufferMemory);

        device.CopyBuffer(stagingBuffer, vertexBuffer, bufferSize);
        vkDestroyBuffer(device.GetDevice(), stagingBuffer, nullptr);
        vkFreeMemory(device.GetDevice(), stagingBufferMemory, nullptr);
    }

    void Model::CreateIndexBuffer(const std::vector<uint32_t>& indices)
    {
        indexCount = static_cast<uint32_t>(indices.size());
        hasIndexBuffer = indexCount > 0;
        if (!hasIndexBuffer)
            return;
        VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;


        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        device.CreateBuffer(
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer,
            stagingBufferMemory);

        void* data;
        if (vkMapMemory(device.GetDevice(), stagingBufferMemory, 0, bufferSize, 0, &data) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to map memory");
        }
        memcpy(data, indices.data(), bufferSize);
        vkUnmapMemory(device.GetDevice(), stagingBufferMemory);


        device.CreateBuffer(
            bufferSize,
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            indexBuffer,
            indexBufferMemory);

        device.CopyBuffer(stagingBuffer, indexBuffer, bufferSize);
        vkDestroyBuffer(device.GetDevice(), stagingBuffer, nullptr);
        vkFreeMemory(device.GetDevice(), stagingBufferMemory, nullptr);
    }


    void Model::Bind(VkCommandBuffer commandBuffer)
    {
        VkBuffer buffers[] = { vertexBuffer };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
        if (hasIndexBuffer)
        {
            vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
        }
    }

    void Model::Draw(VkCommandBuffer commandBuffer)
    {
        if (hasIndexBuffer)
        {
            vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
        }
        else
        {
            vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
        }
    }

    std::vector<VkVertexInputBindingDescription> Model::Vertex::GetBindingDescription()
    {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(Vertex);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescriptions;
    }

    std::vector<VkVertexInputAttributeDescription> Model::Vertex::GetAttributeDescriptions()
    {
        std::vector<VkVertexInputAttributeDescription> attribute(2);
        attribute[0].binding = 0;
        attribute[0].offset = offsetof(Vertex, position);
        attribute[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attribute[0].location = 0;

        attribute[1].binding = 0;
        attribute[1].offset = offsetof(Vertex,color);
        attribute[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attribute[1].location = 1;

        return attribute;
    }

    std::unique_ptr<Model> Model::CreateModelFromFile(Device& device, const std::string& filepath)
    {
        ModelData modelData{};
        modelData.LoadModel(filepath);

        return std::make_unique<Model>(device, modelData);
    }


    void Model::ModelData::LoadModel(const std::string& filepath)
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if(!tinyobj::LoadObj(&attrib,&shapes,&materials,&warn,&err,filepath.c_str()))
        {
            throw std::runtime_error(warn + err);
        }

        vertices.clear();
        indices.clear();

        std::unordered_map<Vertex, uint32_t> uniqueVertices{};
        for(auto &shape : shapes)
        {
            for(auto &index : shape.mesh.indices)
            {
                Vertex vertex{};

                if(index.vertex_index >= 0)
                {
                    vertex.position =
                    {
                        attrib.vertices[3 * index.vertex_index],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2],
                    };

                    auto colorIndex = 3 * index.vertex_index + 2;
                    if(colorIndex < attrib.colors.size())
                    {
                        vertex.color =
                        {
                            attrib.colors[colorIndex - 2],
                            attrib.colors[colorIndex - 1],
                            attrib.colors[colorIndex],
                        };
                    }
                    else
                    {
                        vertex.color = { 1.f,1.f,1.f };
                    }

                    vertex.normal =
                    {
                        attrib.normals[3 * index.normal_index],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2],
                    };

                    vertex.texCord =
                    {
                        attrib.texcoords[2 * index.texcoord_index],
                        attrib.texcoords[2 * index.texcoord_index + 1],
                    };

                    if(uniqueVertices.count(vertex) == 0)
                    {
                        uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                        vertices.push_back(vertex);
                    }

                    indices.push_back(uniqueVertices[vertex]);
                }
            }
        }
    }

}
