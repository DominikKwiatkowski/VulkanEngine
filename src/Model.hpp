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
    /// <summary>
    /// Class represent object data in terms of rendering
    /// </summary>
    class Model
    {
    public:
        /// <summary>
        /// Struct represents single Vertex data
        /// </summary>
        struct Vertex
        {
            glm::vec3 position;
            glm::vec3 color;
            glm::vec3 normal;
            glm::vec2 texCord;

            /// <summary>
            /// Fetch default binding description.
            /// </summary>
            /// <returns>std::vector<VkVertexInputBindingDescription> bindingDescriptions</returns>
            static std::vector<VkVertexInputBindingDescription> GetBindingDescription();

            /// <summary>
            /// Fetch default attribute description
            /// </summary>
            /// <returns>vector<VkVertexInputAttributeDescription> array of binding description</returns>
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

        Model(Device& device, const ModelData& builder);
        ~Model();
        Model(const Model&) = delete;
        Model& operator=(const Model&) = delete;

        /// <summary>
        /// Load model from file using tiny_obj_loader and create model object
        /// </summary>
        /// <param name="device"> Current device</param>
        /// <param name="filepath"> Path to model data</param>
        /// <returns>unique_ptr<Model> created model</returns>
        static std::unique_ptr<Model> CreateModelFromFile(Device& device, const std::string& filepath);

        /// <summary>
        /// Bind model to commandBuffer
        /// </summary>
        /// <param name="commandBuffer"> Current command buffer</param>
        void Bind(VkCommandBuffer commandBuffer);

        /// <summary>
        /// Record draw to commandBuffer
        /// </summary>
        /// <param name="commandBuffer"> Current command buffer</param>
        void Draw(VkCommandBuffer commandBuffer);

    private:
        /// <summary>
        /// Create vertex buffer.
        /// </summary>
        /// <param name="vertices"> Vertices to be write to vertex buffer</param>
        void CreateVertexBuffer(const std::vector<Vertex>& vertices);

        /// <summary>
        /// Create index buffer.
        /// </summary>
        /// <param name="indices">Indices to be write to index buffer</param>
        void CreateIndexBuffer(const std::vector<uint32_t>& indices);

        Device& device;

        std::unique_ptr<Buffer> vertexBuffer;
        uint32_t vertexCount;

        bool hasIndexBuffer = false;
        std::unique_ptr<Buffer> indexBuffer;
        uint32_t indexCount;
    };
}
