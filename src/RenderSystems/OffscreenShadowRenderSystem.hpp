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
#include "Descriptors.hpp"
#include "FrameInfo.hpp"
#include "RenderSystem.hpp"

namespace VulkanEngine
{
    /// <summary>
    /// Class to render shadows to offscreen renderer
    /// </summary>
    class OffscreenShadowRenderSystem : public RenderSystem
    {
    public:
        OffscreenShadowRenderSystem(Device& device, VkRenderPass renderPass, std::vector<VkDescriptorSetLayout> descriptorSetLayouts);
        /// <summary>
        /// Render all objects inside frameInfo.
        /// </summary>
        /// <param name="frameInfo"> Information about current frame</param>
        void Render(FrameInfo frameInfo) override;

    private:
        void CreatePipelineLayout(std::vector<VkDescriptorSetLayout> descriptorSetLayouts);
        void CreatePipeline(VkRenderPass renderPass);

        struct PushConstantData
        {
            glm::mat4 viewMatrix{ 1.f };
        };
    };
}
