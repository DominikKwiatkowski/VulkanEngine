#pragma once

#include "Model.hpp"

#include <memory>

namespace VulkanEngine
{
    struct Transfrom2dComponent
    {
        glm::vec2 translation{};
        glm::vec2 scale{1.f,1.f};
        float rotation = 0;

        glm::mat2 mat2()
        {
            const float sin = glm::sin(rotation);
            const float cos = glm::cos(rotation);

            glm::mat2 rotMatrix = { {cos,sin},{-sin,cos} };
            glm::mat2 scaleMat{ {scale.x,0.0f},{0.f,scale.y} };
            return rotMatrix * scaleMat;
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
        Transfrom2dComponent transform2d{};

    private:
        GameObject(id_t id) : id(id){}

        id_t id;
    };
}