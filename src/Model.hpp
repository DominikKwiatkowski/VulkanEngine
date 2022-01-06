#pragma once
#include "Device.hpp"
#include "Buffer.hpp"
// Glm
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <memory>
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
            glm::vec3 normal;
            glm::vec2 texCord;
            static std::vector<VkVertexInputBindingDescription> GetBindingDescription();
            static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();

            bool operator==(const Vertex& second) const
            {
                return position == second.position &&
                    color == second.color &&
                    normal == second.normal &&
                    texCord == second.texCord;
            }
        };

        struct ModelData
        {
            std::vector<Vertex> vertices{};
            std::vector<uint32_t> indices{};

            void LoadModel(const std::string& filepath);
        };

        Model(Device &device, const ModelData &builder);
        ~Model();
        Model(const Model&) = delete;
        Model &operator=(const Model&) = delete;

        static std::unique_ptr<Model> CreateModelFromFile(Device& device, const std::string& filepath);

        void Bind(VkCommandBuffer commandBuffer);
        void Draw(VkCommandBuffer commandBuffer);
    private:
        void CreateVertexBuffer(const std::vector<Vertex>& vertices);
        void CreateIndexBuffer(const std::vector<uint32_t> &indices);

        Device& device;

        std::unique_ptr<Buffer> vertexBuffer;
        uint32_t vertexCount;

        bool hasIndexBuffer = false;
        std::unique_ptr<Buffer> indexBuffer;
        uint32_t indexCount;

    };
}
