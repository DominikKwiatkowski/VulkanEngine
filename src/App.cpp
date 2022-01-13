#pragma once
#include "App.hpp"
#include "RenderSystems/ObjectRenderSystem.hpp"
#include "RenderSystems/PointLightSystem.hpp"
#include <array>
#include <chrono>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include "Buffer.hpp"
#include "Image.hpp"
#include "Terrain.hpp"

namespace VulkanEngine
{
    struct GlobalUbo
    {
        glm::mat4 projectionMatrix{1.f};
        glm::mat4 viewMatrix{ 1.f };
        // intensity is w light color component
        glm::vec4 ambientLight{ 1.f,1.f,1.f,0.02f };
        glm::vec4 lightPosition{-1.f};
        glm::vec4 lightColor{ 1.f };
    };

    App::App()
    {
        globalPool = DescriptorPool::Builder(device)
            .SetMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
            .AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
            .AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, SwapChain::MAX_FRAMES_IN_FLIGHT)
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
            .AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
            .AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,VK_SHADER_STAGE_FRAGMENT_BIT)
            .Build();

        std::vector<VkDescriptorSet> globalDescriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);
        auto texture = Image::LoadImageFromFile("../textures/vase_texture.jpg", device);
        auto textureInfo = texture->GetDescriptorInfo(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        for(int i = 0; i< globalDescriptorSets.size();i++)
        {
            auto bufferInfo = uboBuffers[i]->DescriptorInfo();

            DescriptorWriter(*globalSetLayout, *globalPool)
                .WriteBuffer(0, &bufferInfo)
                .WriteImage(1,&textureInfo)
                .Build(globalDescriptorSets[i]);
        }

        Camera camera{};

        std::vector<std::unique_ptr<RenderSystem>> renderSystems;

        // Ad object render system
        renderSystems.push_back(std::make_unique<ObjectRenderSystem>(
            device, renderer.getSwapChainRenderPass(),globalSetLayout->GetDescriptorSetLayout() ));

        // Add point light render system
        renderSystems.push_back(std::make_unique<PointLightSystem>(
            device, renderer.getSwapChainRenderPass(),globalSetLayout->GetDescriptorSetLayout() ));

        auto cameraObject = GameObject::CreateGameObject();
        cameraObject.transform.translation.z = -2.5f;
        KeyboardController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();
        while (!window.ShouldClose())
        {
            // Calculate time between frames
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
                FrameInfo frameInfo{ frameIndex, frameTime, camera, commandBuffer, globalDescriptorSets[frameIndex], gameObjects};

                GlobalUbo ubo{};

                ubo.projectionMatrix = camera.GetProjectionMatrix();
                ubo.viewMatrix = camera.GetViewMatrix();
                uboBuffers[frameIndex]->WriteToBuffer(&ubo);

                renderer.BeginSwapChainRenderPass(commandBuffer);

                // Each render system will render this frame
                for (auto &renderSystem : renderSystems)
                {
                    renderSystem->Render(frameInfo);
                }

                renderer.EndSwapChainRenderPass(commandBuffer);
                renderer.EndFrame();
            }
        }

        vkDeviceWaitIdle(device.GetDevice());
    }

    void App::LoadGameObjects()
    {
        std::shared_ptr flatModel = Model::CreateModelFromFile(device, "../models/flat_vase.obj");
        std::shared_ptr smoothModel = Model::CreateModelFromFile(device, "../models/smooth_vase.obj");
        std::shared_ptr floorModel = Model::CreateModelFromFile(device, "../models/quad.obj");

        auto flatVase = GameObject::CreateGameObject();
        flatVase.model = flatModel;
        flatVase.transform.translation = {0.5, 0.5, 0};
        flatVase.transform.scale = {3, 1.5, 3};
        gameObjects.emplace(flatVase.GetId(),std::move(flatVase));

        auto smoothVase = GameObject::CreateGameObject();
        smoothVase.model = smoothModel;
        smoothVase.transform.translation = { -0.5, 0.5, 0 };
        smoothVase.transform.scale = { 3, 1.5, 3 };
        gameObjects.emplace(smoothVase.GetId(), std::move(smoothVase));

        auto floor = GameObject::CreateGameObject();
        floor.model = floorModel;
        floor.transform.translation = { 0 ,0.5, 0 };
        floor.transform.scale = { 5,1,5 };
        gameObjects.emplace(floor.GetId(), std::move(floor));


        // auto terrain = GameObject::CreateGameObject();
        // terrain.model = Terrain::Generate(device, 1000);
        // terrain.transform.translation = { -5 ,0, -5 };
        // terrain.transform.scale = { 10,1,10 };
        // gameObjects.emplace(terrain.GetId(), std::move(terrain));
    }


}
