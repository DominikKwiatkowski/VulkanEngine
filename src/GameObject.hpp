#pragma once

#include "Model.hpp"

#include <memory>
#include <unordered_map>
#include <glm/gtc/matrix_transform.hpp>
namespace VulkanEngine
{
    struct TransformComponent
    {
        glm::vec3 translation{};
        glm::vec3 scale{1.f};
        glm::vec3 rotation = {};

        glm::mat4 GetTransformationMatrix();
        glm::mat3 GetNormalTransformationMatrix();
    };


    class GameObject
    {
    public:
        using id_t = unsigned int;
        using Map = std::unordered_map<id_t, GameObject>;
        static GameObject CreateGameObject();

        id_t GetId() { return id; }

        GameObject(const GameObject&) = delete;
        GameObject& operator=(const GameObject&) = delete;
        GameObject(GameObject&&) = default;
        GameObject& operator=(GameObject&&) = default;

        std::shared_ptr<Model> model{};
        glm::vec3 color{};
        TransformComponent transform{};

    private:
        GameObject(id_t id) : id(id){}

        id_t id;
    };
}