#pragma once
#include "App.hpp"

#include <array>
#include <stdexcept>

namespace VulkanEngine
{
    struct PushConstantData
    {
        glm::mat2 transform{ 1.f };
        glm::vec2 offsetField;
        alignas(16)glm::vec3 color;
    };

    App::App()
    {
        LoadGameObjects();
        CreatePipelineLayout();
        RecreateSwapChain();
        CreateCommandBuffer();
    }

    App::~App()
    {
        vkDestroyPipelineLayout(device.GetDevice(), pipelineLayout, nullptr);
    }


    void App::run()
    {
        while(!window.ShouldClose())
        {
            glfwPollEvents();
            DrawFrame();
        }

        vkDeviceWaitIdle(device.GetDevice());
    }

    void App::CreatePipelineLayout()
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
        if(vkCreatePipelineLayout(device.GetDevice(),&pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to pipeline layout!");
        }
    }

    void App::CreatePipeline()
    {
        PipelineConfigInfo pipelineConfig{};
        Pipeline::DefaultPipelineConfigInfo(
            pipelineConfig);

        pipelineConfig.renderPass = swapChain->GetRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout;
        pipeline = std::make_unique<Pipeline>(
            device,
            "Shaders/vert_shader.vert.spv",
            "Shaders/frag_shader.frag.spv",
            pipelineConfig);
    }

    void App::CreateCommandBuffer()
    {
        commandBuffers.resize(swapChain->ImageCount());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = device.GetCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if (vkAllocateCommandBuffers(device.GetDevice(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate command buffers");
        }
    }

    void App::FreeCommandBuffer()
    {
        if(commandBuffers.size() >0)
            vkFreeCommandBuffers(
                device.GetDevice(),
                device.GetCommandPool(),
                static_cast<uint32_t>(commandBuffers.size()),
                commandBuffers.data());
    }


    void App::RecordCommandBuffer(int imageIndex)
    {
        VkCommandBufferBeginInfo beginInfo{};

        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to start recording command buffer");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = swapChain->GetRenderPass();
        renderPassInfo.framebuffer = swapChain->GetFrameBuffer(imageIndex);

        renderPassInfo.renderArea.offset = { 0,0 };
        renderPassInfo.renderArea.extent = swapChain->GetSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = { 0.01f,0.01f,0.01f,1.0f };
        clearValues[1].depthStencil = { 1.0f,0 };
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(swapChain->GetSwapChainExtent().width);
        viewport.height = static_cast<float>(swapChain->GetSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{ {0, 0}, swapChain->GetSwapChainExtent() };
        vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
        vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

        RenderGameObjects(commandBuffers[imageIndex]);

        vkCmdEndRenderPass(commandBuffers[imageIndex]);
        if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to stop recording command buffer");
        }
    }

    void App::RenderGameObjects(VkCommandBuffer commandBuffer)
    {
        pipeline->Bind(commandBuffer);

        for(auto& obj : gameObjects)
        {
            obj.transfrom2d.rotation = obj.transfrom2d.rotation + .0001f * glm::two_pi<float>();
            PushConstantData push{};
            push.offsetField = obj.transfrom2d.translation;
            push.color = obj.color;
            push.transform = obj.transfrom2d.mat2();

            vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstantData), &push);
            obj.model->Bind(commandBuffer);
            obj.model->Draw(commandBuffer);
        }
    }


    void App::RecreateSwapChain()
    {
        auto extent = window.getExtent();
        while(extent.width == 0 || extent.height == 0)
        {
            extent = window.getExtent();
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(device.GetDevice());
        if(swapChain != nullptr)
        {
            swapChain = std::make_unique<SwapChain>(device, extent, std::move(swapChain));
        }
        else
        {
            swapChain = std::make_unique<SwapChain>(device, extent);
        }
        if(swapChain->ImageCount() != commandBuffers.size())
        {
            FreeCommandBuffer();
            CreateCommandBuffer();
        }
        CreatePipeline();
    }


    void App::DrawFrame()
    {
        uint32_t imageIndex;
        auto result = swapChain->AcquireNextImage(&imageIndex);

        if(result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            RecreateSwapChain();
        }
        if(result != VK_SUCCESS && result!=VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("failed to acquire next image");
        }

        RecordCommandBuffer(imageIndex);
        result = swapChain->SubmitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
        if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.WasWindowResized())
        {
            window.ResetWindowResizedFlag();
            RecreateSwapChain();
        }
    }

    void App::LoadGameObjects()
    {
        std::vector<glm::vec3> colors{
        {1.f, .7f, .73f},
        {1.f, .87f, .73f},
        {1.f, 1.f, .73f},
        {.73f, 1.f, .8f},
        {.73, .88f, 1.f}  //
        };
        std::vector<Model::Vertex> vertices{
            {{0,-0.25},{1,0,0}},
            {{0.25,0.25},{0,1,0}},
            {{-0.25,0.25},{0,0,1}}
        };
        // std::vector<Model::Vertex> vertices = {};
        // Sierpinski(vertices, 5, { 0,-0.5 }, { 0.5,0.5 }, { -0.5,0.5 });


        auto model = std::make_shared<Model>(device, vertices);

        auto triangle = GameObject::CreateGameObject();
        triangle.model = model;
        triangle.color = { .1,.8,.1 };
        triangle.transfrom2d.translation.x = .2f;
        triangle.transfrom2d.scale.x = 0.5f;
        triangle.transfrom2d.scale.y = 1.5f;
        triangle.transfrom2d.rotation = .25f * glm::two_pi<float>();

        gameObjects.push_back(std::move(triangle));
    }

    void App::Sierpinski(std::vector<Model::Vertex>& vertices,
        int depth,
        glm::vec2 left,
        glm::vec2 right,
        glm::vec2 top)
    {
        if(depth <= 0)
        {
            vertices.push_back({left,{1,0,1} });
            vertices.push_back({ right ,{0,1,0} });
            vertices.push_back({ top,{0,0,1} });
        }
        else
        {
            auto leftTop = 0.5f * (left + top);
            auto rightTop = 0.5f * (right + top);
            auto leftRight = 0.5f * (left + right);

            Sierpinski(vertices, depth - 1, left, leftRight, leftTop);
            Sierpinski(vertices, depth - 1, leftRight, right, rightTop);
            Sierpinski(vertices, depth - 1, leftTop, rightTop, top);
        }
    }

}
