#pragma once

#include "Model.hpp"

#include <memory>
#include <glm/gtc/matrix_transform.hpp>
namespace VulkanEngine
{
    struct TransfromComponent
    {
        glm::vec3 translation{};
        glm::vec3 scale{1.f};
        glm::vec3 rotation = {};

        glm::mat4 mat4()
        {
            auto transform = glm::translate(glm::mat4{ 1.f }, translation);
            transform = glm::rotate(transform, rotation.y, {0.0, 1.0, 0.0});
            transform = glm::rotate(transform, rotation.x, { 1.0, 0.0, 0.0 });
            transform = glm::rotate(transform, rotation.z, { 0.0, 0.0, 1.0 });
            transform = glm::scale(transform, scale);
            return transform;
        }
    };


    class GameObject
    {
    public:
        using id_t = unsigned int;

        static GameObject CreateGameObject()
        {
            static id_t currentId = 0;
            return GameObject(currentId++);
        }

        id_t GetId() { return id; }

        GameObject(const GameObject&) = delete;
        GameObject& operator=(const GameObject&) = delete;
        GameObject(GameObject&&) = default;
        GameObject& operator=(GameObject&&) = default;

        std::shared_ptr<Model> model{};
        glm::vec3 color{};
        TransfromComponent transform{};

    private:
        GameObject(id_t id) : id(id){}

        id_t id;
    };
}