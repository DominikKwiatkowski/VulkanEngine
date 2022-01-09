#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace VulkanEngine
{
    /// <summary>
    /// Class describing camera and perform all calculations.
    /// </summary>
    class Camera
    {
    public:
        /// <summary>
        /// Set projection matrix to be in ortographic projection.
        /// </summary>
        /// <param name="left"> Left border position </param>
        /// <param name="right"> Right border position</param>
        /// <param name="top"> Top border position</param>
        /// <param name="bottom"> Bottom border position</param>
        /// <param name="near"> Near border position</param>
        /// <param name="far"> Far border position</param>
        void SetOrtographicProjection(
            float left, float right, float top,
            float bottom, float near, float far);

        /// <summary>
        /// Set projection matrix to be in perspective projection.
        /// </summary>
        /// <param name="fovy"> Set field of view</param>
        /// <param name="aspect"> Set aspect of display</param>
        /// <param name="near"> Near border position</param>
        /// <param name="far"> Far border position</param>
        void SetPerspectiveProjection(float fovy, float aspect, float near, float far);

        /// <summary>
        /// Set camera to look at certain view.
        /// </summary>
        /// <param name="position"> Position of camera</param>
        /// <param name="direction"> Direction of camera</param>
        /// <param name="up">Up vector, by default fix vulkan y coordinate</param>
        void SetViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up = {0.f, -1.0f, 0.f});

        /// <summary>
        /// Set view to look at specific target.
        /// </summary>
        /// <param name="position"> Position of camera</param>
        /// <param name="target"> Target position</param>
        /// <param name="up">Up vector, by default fix vulkan y coordinate</param>
        void SetViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up = {0.f, -1.0f, 0.f});

        /// <summary>
        /// Set view by YXZ position.
        /// </summary>
        /// <param name="position"> Camera position</param>
        /// <param name="rotation"> Camera rotation</param>
        void SetViewYXZ(glm::vec3 position, glm::vec3 rotation);

        const glm::mat4& GetProjectionMatrix() const
        {
            return projectionMatrix;
        }

        const glm::mat4& GetViewMatrix() const
        {
            return viewMatrix;
        }

    private:
        glm::mat4 projectionMatrix{1.0f};
        glm::mat4 viewMatrix{1.f};
    };
}
