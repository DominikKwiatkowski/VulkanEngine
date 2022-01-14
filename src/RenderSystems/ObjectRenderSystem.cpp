#pragma once
#include "ObjectRenderSystem.hpp"

#include <array>
#include <stdexcept>

#include "Descriptors.hpp"

namespace VulkanEngine
{
    ObjectRenderSystem::ObjectRenderSystem(Device& device, VkRenderPass renderPass,
        std::vector<VkDescriptorSetLayout> descriptorSetLayouts, std::shared_ptr<DescriptorPool> globalPool):
        RenderSystem(device), globalPool(globalPool)
    {
        CreatePipelineLayout(descriptorSetLayouts);
        CreatePipeline(renderPass);
    }

    void ObjectRenderSystem::CreatePipelineLayout(std::vector<VkDescriptorSetLayout> descriptorSetLayouts)
    {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.size = sizeof(PushConstantData);
        pushConstantRange.offset = 0;

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

    void ObjectRenderSystem::CreatePipeline(VkRenderPass renderPass)
    {
        PipelineConfigInfo pipelineConfig{};
        Pipeline::DefaultPipelineConfigInfo(
            pipelineConfig);

        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout;
        pipeline = std::make_unique<Pipeline>(
            device,
            "../Shaders/vert_shader.vert.spv",
            "../Shaders/frag_shader.frag.spv",
            pipelineConfig);
    }

    void ObjectRenderSystem::Render(FrameInfo frameInfo)
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



        for (auto& kv : frameInfo.gameObjects)
        {
            PushConstantData push{};
            push.modelMatrix = kv.second.transform.GetTransformationMatrix();
            push.normalMatrix = kv.second.transform.GetNormalTransformationMatrix();
            push.hasTexture = kv.second.texture != nullptr;
            if(push.hasTexture)
            {
                vkCmdBindDescriptorSets(
                    frameInfo.commandBuffer,
                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                    pipelineLayout,
                    1,
                    1,
                    &kv.second.descriptorSet,
                    0,
                    nullptr);
            }
            vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout,
                               VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstantData),
                               &push);

            kv.second.model->Bind(frameInfo.commandBuffer);
            kv.second.model->Draw(frameInfo.commandBuffer);
        }
    }
}
