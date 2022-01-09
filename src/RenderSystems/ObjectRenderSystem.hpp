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
    class ObjectRenderSystem
    {
    public:
        ObjectRenderSystem(Device &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
        ~ObjectRenderSystem();
        ObjectRenderSystem(const ObjectRenderSystem&) = delete;
        ObjectRenderSystem& operator=(const ObjectRenderSystem&) = delete;

        void RenderGameObjects(FrameInfo frameInfo);
    private:
        Device &device;
        std::unique_ptr<Pipeline> pipeline;
        VkPipelineLayout pipelineLayout;

        void CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout);
        void CreatePipeline(VkRenderPass renderPass);

        struct PushConstantData
        {
            glm::mat4 modelMatrix{ 1.f };
            glm::mat4 normalMatrix{ 1.f };
        };
    };
}
