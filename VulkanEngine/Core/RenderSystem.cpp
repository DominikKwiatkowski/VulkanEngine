#pragma once
#include "RenderSystem.hpp"

#include <array>
#include <stdexcept>

namespace VulkanEngine
{
    struct PushConstantData
    {
        glm::mat4 transform{ 1.f };
        alignas(16)glm::vec3 color;
    };

    RenderSystem::RenderSystem(Device &device, VkRenderPass renderPass):
        device(device)
    {
        CreatePipelineLayout();
        CreatePipeline(renderPass);
    }

    RenderSystem::~RenderSystem()
    {
        vkDestroyPipelineLayout(device.GetDevice(), pipelineLayout, nullptr);
    }


    void RenderSystem::CreatePipelineLayout()
    {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.size = sizeof(PushConstantData);
        pushConstantRange.offset = 0;

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
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

    void RenderSystem::RenderGameObjects(VkCommandBuffer commandBuffer, std::vector<GameObject>& gameObjects)
    {
        pipeline->Bind(commandBuffer);
        for (auto& obj : gameObjects)
        {
            obj.transform.rotation.y = glm::mod(obj.transform.rotation.y + 0.001f, glm::two_pi<float>());
            obj.transform.rotation.x = glm::mod(obj.transform.rotation.x + 0.0005f, glm::two_pi<float>());
            PushConstantData push{};
            push.color = obj.color;
            push.transform = obj.transform.mat4();

            vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstantData), &push);
            obj.model->Bind(commandBuffer);
            obj.model->Draw(commandBuffer);
        }
    }
}
