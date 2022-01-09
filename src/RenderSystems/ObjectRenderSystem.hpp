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
#include "RenderSystem.hpp"

namespace VulkanEngine
{
    class ObjectRenderSystem : public RenderSystem
    {
    public:
        ObjectRenderSystem(Device &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);

        void Render(FrameInfo frameInfo) override;

    private:
        void CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout);
        void CreatePipeline(VkRenderPass renderPass);

        struct PushConstantData
        {
            glm::mat4 modelMatrix{ 1.f };
            glm::mat4 normalMatrix{ 1.f };
        };
    };
}
