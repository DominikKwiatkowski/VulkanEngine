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
    /// <summary>
    /// Helper hashing function.
    /// </summary>
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
    }

    void Model::CreateVertexBuffer(const std::vector<Vertex>& vertices)
    {
        // Calculate vertex data
        vertexCount = static_cast<uint32_t>(vertices.size());
        assert(vertexCount >= 3);
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;

        // Create staging buffer.
        Buffer stagingBuffer(
            device,
            sizeof(vertices[0]),
            vertexCount,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        stagingBuffer.Map();
        stagingBuffer.WriteToBuffer((void *)vertices.data(), bufferSize);

        // Create vertex buffer.
        vertexBuffer = std::make_unique<Buffer>(device,
            sizeof(vertices[0]),
            vertexCount,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        // Copy staging to vertex.
        device.CopyBuffer(stagingBuffer.GetBuffer(), vertexBuffer->GetBuffer(), bufferSize);
    }

    void Model::CreateIndexBuffer(const std::vector<uint32_t>& indices)
    {
        indexCount = static_cast<uint32_t>(indices.size());
        hasIndexBuffer = indexCount > 0;

        // If no index buffer provided, return
        if (!hasIndexBuffer)
            return;

        VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;

        // Create staging buffer
        Buffer stagingBuffer(
            device,
            sizeof(indices[0]),
            indexCount,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        stagingBuffer.Map();
        stagingBuffer.WriteToBuffer((void*)indices.data(), bufferSize);

        // Create index buffer
        indexBuffer = std::make_unique<Buffer>(device,
            sizeof(indices[0]),
            indexCount,
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        // Copy staging buffer to index
        device.CopyBuffer(stagingBuffer.GetBuffer(), indexBuffer->GetBuffer(), bufferSize);
    }


    void Model::Bind(VkCommandBuffer commandBuffer)
    {
        VkBuffer buffers[] = { vertexBuffer->GetBuffer() };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
        if (hasIndexBuffer)
        {
            vkCmdBindIndexBuffer(commandBuffer, indexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
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
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
        attributeDescriptions.push_back({ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position) });
        attributeDescriptions.push_back({ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color) });
        attributeDescriptions.push_back({ 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal) });
        attributeDescriptions.push_back({ 3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, texCord) });

        return attributeDescriptions;
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

        // Don't want to duplicate vertices due to performance issue,
        // so store only unique vertices and write proper index
        std::unordered_map<Vertex, uint32_t> uniqueVertices{};

        // For each shape in file, load it.
        for(auto &shape : shapes)
        {
            // For each index in shape index
            for(auto &index : shape.mesh.indices)
            {
                Vertex vertex{};

                // Read vertex data
                if(index.vertex_index >= 0)
                {
                    vertex.position =
                    {
                        attrib.vertices[index.vertex_index * 3],
                        attrib.vertices[index.vertex_index * 3 + 1],
                        attrib.vertices[index.vertex_index * 3 + 2],
                    };

                    vertex.color =
                    {
                        attrib.colors[index.vertex_index * 3],
                        attrib.colors[index.vertex_index * 3 + 1],
                        attrib.colors[index.vertex_index * 3 + 2],
                    };

                    vertex.normal =
                    {
                        attrib.normals[index.normal_index * 3],
                        attrib.normals[index.normal_index * 3 + 1],
                        attrib.normals[index.normal_index * 3 + 2],
                    };

                    vertex.texCord =
                    {
                        attrib.texcoords[index.texcoord_index * 2],
                        attrib.texcoords[index.texcoord_index * 2 + 1],
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
