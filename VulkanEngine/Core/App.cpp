#pragma once
#include "App.hpp"
#include "RenderSystem.hpp"
#include <array>
#include <stdexcept>

namespace VulkanEngine
{

    App::App()
    {
        LoadGameObjects();
    }

    App::~App()
    {

    }

    void App::run()
    {
        std::vector<glm::vec3> colors{
        {1.f, 0.f, 0.f},
        {0.f, 1.0f, 0.f} //
        };

        RenderSystem renderSystem{ device,renderer.getSwapChainRenderPass() };
        while(!window.ShouldClose())
        {
            glfwPollEvents();
            if(auto commandBuffer = renderer.BeginFrame())
            {
                renderer.BeginSwapChainRenderPass(commandBuffer);
                renderSystem.RenderGameObjects(commandBuffer, gameObjects);
                renderer.EndSwapChainRenderPass(commandBuffer);
                renderer.EndFrame();
            }
        }

        vkDeviceWaitIdle(device.GetDevice());
    }

    void App::LoadGameObjects() {
        std::vector<Model::Vertex> vertices{
            {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
            {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}} };
        auto lveModel = std::make_shared<Model>(device, vertices);

        auto triangle = GameObject::CreateGameObject();
        triangle.model = lveModel;
        triangle.color = { .1f, .8f, .1f };
        triangle.transform2d.translation.x = .2f;
        triangle.transform2d.scale = { 2.f, .5f };
        triangle.transform2d.rotation = .25f * glm::two_pi<float>();

        gameObjects.push_back(std::move(triangle));
    }


}
