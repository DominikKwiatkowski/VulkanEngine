#pragma once

#include "Model.hpp"

#include <memory>
#include <unordered_map>
#include <glm/gtc/matrix_transform.hpp>

#include "Image.hpp"

namespace VulkanEngine
{
    struct TransformComponent
    {
        glm::vec3 translation{};
        glm::vec3 scale{1.f};
        glm::vec3 rotation = {};

        /// <summary>
        /// Calculate and get transformation matrix.
        /// </summary>
        /// <returns> glm::mat4 transformation matrix</returns>
        glm::mat4 GetTransformationMatrix();

        /// <summary>
        /// Calculate and get normal transformation matrix.
        /// </summary>
        /// <returns> glm::mat3 normal transformation</returns>
        glm::mat3 GetNormalTransformationMatrix();
    };

    class GameObject
    {
    public:
        using id_t = unsigned int;
        using Map = std::unordered_map<id_t, GameObject>;

        // Build function to properly asign id to object.
        static GameObject CreateGameObject();

        id_t GetId() { return id; }

        // Delete copy and move operators.
        GameObject(const GameObject&) = delete;
        GameObject& operator=(const GameObject&) = delete;
        GameObject(GameObject&&) = default;
        GameObject& operator=(GameObject&&) = default;

        std::shared_ptr<Model> model{};
        std::shared_ptr<Image> texture{};
        glm::vec3 color{};
        TransformComponent transform{};
        VkDescriptorSet descriptorSet;

    private:
        GameObject(id_t id) : id(id)
        {
        }

        id_t id;
    };
}
