#include "KeyboardController.hpp"


namespace VulkanEngine
{
    void KeyboardController::MoveInPlane(Window& window, float dt, GameObject& object)
    {
        // Check if windows resized. In this case camera should not move due to calculation breaks.
        if ((abs(OldWindowSize.x - window.getExtent().width) < glm::epsilon<double>()) &&
            (abs(OldWindowSize.y - window.getExtent().height) < glm::epsilon<double>()))
        {
            // Calculate move, note that mouse cursor will always be centralized.
            auto xMove = (static_cast<int>(window.getExtent().width / 2) - window.GetMousePos().x) / window.getExtent().
                width * 10;
            auto yMove = (static_cast<int>(window.getExtent().height / 2) - window.GetMousePos().y) / window.getExtent()
                .height * 10;

            glm::vec3 rotate{0};

            rotate.x += static_cast<float>(yMove);
            rotate.y -= static_cast<float>(xMove);

            // Check length of vector > 0
            if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon())
                object.transform.rotation += lookSpeed * glm::normalize(rotate) * dt;

            object.transform.rotation.x = glm::clamp(object.transform.rotation.x, -1.5f, 1.5f);
            object.transform.rotation.y = glm::mod(object.transform.rotation.y, glm::two_pi<float>());
        }

        // Center mouse position.
        window.CenterMousePos();

        // Save old window size
        OldWindowSize.x = window.getExtent().width;
        OldWindowSize.y = window.getExtent().height;

        // Calculate move directions.
        float yaw = object.transform.rotation.y;
        const glm::vec3 forwardDir(sin(yaw), 0.f, cos(yaw));
        const glm::vec3 rightDir(forwardDir.z, 0.f, -forwardDir.x);
        const glm::vec3 upDir(0.f, -1.f, 0.f);

        // If key pressed, apply move.
        glm::vec3 moveDir(0.f);
        if (glfwGetKey(window.GetGLFWWindow(), keys.moveForward) == GLFW_PRESS)
        {
            moveDir += forwardDir;
        }
        if (glfwGetKey(window.GetGLFWWindow(), keys.moveBackward) == GLFW_PRESS)
        {
            moveDir -= forwardDir;
        }
        if (glfwGetKey(window.GetGLFWWindow(), keys.moveRight) == GLFW_PRESS)
        {
            moveDir += rightDir;
        }
        if (glfwGetKey(window.GetGLFWWindow(), keys.moveLeft) == GLFW_PRESS)
        {
            moveDir -= rightDir;
        }
        if (glfwGetKey(window.GetGLFWWindow(), keys.moveUp) == GLFW_PRESS)
        {
            moveDir += upDir;
        }
        if (glfwGetKey(window.GetGLFWWindow(), keys.moveDown) == GLFW_PRESS)
        {
            moveDir -= upDir;
        }
        if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon())
            object.transform.translation += moveSpeed * glm::normalize(moveDir) * dt;
    }
}
