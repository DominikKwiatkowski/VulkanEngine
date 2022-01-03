#pragma once
#include "App.hpp"
#include "RenderSystem.hpp"
#include <array>
#include <chrono>
#include <stdexcept>
#include "Buffer.hpp"

namespace VulkanEngine
{
    struct GlobalUbo
    {
        glm::mat4 projectionView{1.f};
        glm::vec3 lightDirection = glm::normalize(glm::vec3(1.f, -3.f, -1.f));
    };

    App::App()
    {
        globalPool = DescriptorPool::Builder(device)
            .SetMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
            .AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
            .Build();

        LoadGameObjects();
    }

    App::~App()
    {
    }

    void App::run()
    {
        std::vector<std::unique_ptr<Buffer>> uboBuffers(SwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < uboBuffers.size(); i++) {
            uboBuffers[i] = std::make_unique<Buffer>(
                device,
                sizeof(GlobalUbo),
                1,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
            uboBuffers[i]->Map();
        }

        auto globalSetLayout = DescriptorSetLayout::Builder(device)
            .AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
            .Build();

        std::vector<VkDescriptorSet> globalDescriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);
        for(int i = 0; i< globalDescriptorSets.size();i++)
        {
            auto bufferInfo = uboBuffers[i]->DescriptorInfo();
            DescriptorWriter(*globalSetLayout, *globalPool)
                .WriteBuffer(0, &bufferInfo)
                .Build(globalDescriptorSets[i]);
        }

        Camera camera{};
        RenderSystem renderSystem{device, renderer.getSwapChainRenderPass(),globalSetLayout->GetDescriptorSetLayout()};

        auto cameraObject = GameObject::CreateGameObject();
        KeyboardController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();
        while (!window.ShouldClose())
        {
            glfwPollEvents();
            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            cameraController.MoveInPlane(window, frameTime, cameraObject);
            camera.SetViewYXZ(cameraObject.transform.translation, cameraObject.transform.rotation);

            float aspect = renderer.GetAspectRatio();
            camera.SetPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 10);

            if (auto commandBuffer = renderer.BeginFrame())
            {
                int frameIndex = renderer.GetFrameIndex();
                FrameInfo frameInfo{ frameIndex, frameTime, camera, commandBuffer, globalDescriptorSets[frameIndex]};

                GlobalUbo ubo{};

                ubo.projectionView = camera.GetProjectionMatrix() * camera.GetViewMatrix();
                uboBuffers[frameIndex]->WriteToBuffer(&ubo);

                renderer.BeginSwapChainRenderPass(commandBuffer);
                renderSystem.RenderGameObjects(frameInfo, gameObjects);
                renderer.EndSwapChainRenderPass(commandBuffer);
                renderer.EndFrame();
            }
        }

        vkDeviceWaitIdle(device.GetDevice());
    }

    void App::LoadGameObjects()
    {
        std::shared_ptr flatModel = Model::CreateModelFromFile(device, "models/flat_vase.obj");
        std::shared_ptr smoothModel = Model::CreateModelFromFile(device, "models/smooth_vase.obj");

        auto flatVase = GameObject::CreateGameObject();
        flatVase.model = flatModel;
        flatVase.transform.translation = {0.25, 0, 1.5};
        flatVase.transform.scale = {0.5, 0.25, 0.5};
        gameObjects.push_back(std::move(flatVase));

        auto smoothVase = GameObject::CreateGameObject();
        smoothVase.model = smoothModel;
        smoothVase.transform.translation = { -0.25, 0, 1.5 };
        smoothVase.transform.scale = { 0.5, 0.25, 0.5 };
        gameObjects.push_back(std::move(smoothVase));
    }
}
