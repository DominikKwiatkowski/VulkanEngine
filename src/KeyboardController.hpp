#pragma once

#include "GameObject.hpp"
#include "Window.hpp"
namespace VulkanEngine
{
    class KeyboardController
    {
    public:
        struct KeyMappings
        {
            int moveLeft = GLFW_KEY_A;
            int moveRight = GLFW_KEY_D;
            int moveForward = GLFW_KEY_W;
            int moveBackward = GLFW_KEY_S;
            int moveUp = GLFW_KEY_E;
            int moveDown = GLFW_KEY_Q;
        };

        void MoveInPlane(Window &window, float dt, GameObject& object);


        KeyMappings keys{};
        float moveSpeed{ 3.f };
        float lookSpeed{ 6.f };

        glm::dvec2 OldWindowSize;
    };
}