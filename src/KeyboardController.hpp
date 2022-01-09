#pragma once

#include "GameObject.hpp"
#include "Window.hpp"

namespace VulkanEngine
{
    /// <summary>
    /// Class to handle user input
    /// </summary>
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

        /// <summary>
        /// Calculate user move and apply it to object.
        /// </summary>
        /// <param name="window"> Current window</param>
        /// <param name="dt"> Time from last frame</param>
        /// <param name="object"> object to be transform by user input</param>
        void MoveInPlane(Window& window, float dt, GameObject& object);


        KeyMappings keys{};
        float moveSpeed{3.f};
        float lookSpeed{6.f};

        glm::dvec2 OldWindowSize;
    };
}
