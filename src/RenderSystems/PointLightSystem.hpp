#pragma once
#include <memory>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "Model.hpp"
#include "Pipeline.hpp"
#include "GameObject.hpp"
#include "Camera.hpp"
#include "FrameInfo.hpp"

namespace VulkanEngine
{
    /// <summary>
    /// System to display light. Point light data is stored in global UBO
    /// </summary>
    class PointLightSystem
    {
    public:
        PointLightSystem(Device &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
        ~PointLightSystem();
        PointLightSystem(const PointLightSystem&) = delete;
        PointLightSystem& operator=(const PointLightSystem&) = delete;

        /// <summary>
        /// Render point light
        /// </summary>
        /// <param name="frameInfo"> Current frame info</param>
        void Render(FrameInfo frameInfo);
    private:
        Device &device;
        std::unique_ptr<Pipeline> pipeline;
        VkPipelineLayout pipelineLayout;

        void CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout);
        void CreatePipeline(VkRenderPass renderPass);
    };
}
