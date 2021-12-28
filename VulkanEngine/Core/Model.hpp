#pragma once
#include "Device.hpp"

// Glm
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
namespace VulkanEngine
{
    class Model
    {
    public:

        struct Vertex
        {
            glm::vec3 position;
            glm::vec3 color;
            static std::vector<VkVertexInputBindingDescription> getBindingDescription();
            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
        };

        Model(Device &device, const std::vector<Vertex>& vertices);
        ~Model();
        Model(const Model&) = delete;
        Model &operator=(const Model&) = delete;

        void Bind(VkCommandBuffer commandBuffer);
        void Draw(VkCommandBuffer commandBuffer);
    private:
        void CreateVertexBuffer(const std::vector<Vertex>& vertices);
        Device& device;
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        uint32_t vertexCount;

    };
}
