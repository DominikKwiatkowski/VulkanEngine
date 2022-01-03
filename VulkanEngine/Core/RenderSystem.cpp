#pragma once
#include "RenderSystem.hpp"

#include <array>
#include <stdexcept>

namespace VulkanEngine
{
    struct PushConstantData
    {
        glm::mat4 modelMatrix{1.f};
        glm::mat4 normalMatrix{1.f};
    };

    RenderSystem::RenderSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout):
        device(device)
    {
        CreatePipelineLayout(globalSetLayout);
        CreatePipeline(renderPass);
    }

    RenderSystem::~RenderSystem()
    {
        vkDestroyPipelineLayout(device.GetDevice(), pipelineLayout, nullptr);
    }


    void RenderSystem::CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout)
    {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.size = sizeof(PushConstantData);
        pushConstantRange.offset = 0;

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts{};
        descriptorSetLayouts.push_back(globalSetLayout);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
        if (vkCreatePipelineLayout(device.GetDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to pipeline layout!");
        }
    }

    void RenderSystem::CreatePipeline(VkRenderPass renderPass)
    {
        PipelineConfigInfo pipelineConfig{};
        Pipeline::DefaultPipelineConfigInfo(
            pipelineConfig);

        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout;
        pipeline = std::make_unique<Pipeline>(
            device,
            "Shaders/vert_shader.vert.spv",
            "Shaders/frag_shader.frag.spv",
            pipelineConfig);
    }

    void RenderSystem::RenderGameObjects(FrameInfo frameInfo, std::vector<GameObject>& gameObjects)
    {
        pipeline->Bind(frameInfo.commandBuffer);

        vkCmdBindDescriptorSets(
            frameInfo.commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipelineLayout,
            0,
            1,
            &frameInfo.globalDescriptorSet,
            0,
            nullptr);

        for (auto& obj : gameObjects)
        {
            PushConstantData push{};
            push.modelMatrix = obj.transform.GetTransformationMatrix();
            push.normalMatrix = obj.transform.GetNormalTransformationMatrix();

            vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout,
                               VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstantData),
                               &push);
            obj.model->Bind(frameInfo.commandBuffer);
            obj.model->Draw(frameInfo.commandBuffer);
        }
    }
}
