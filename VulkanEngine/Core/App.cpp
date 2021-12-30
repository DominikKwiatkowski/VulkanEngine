#pragma once
#include "App.hpp"
#include "RenderSystem.hpp"
#include <array>
#include <chrono>
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
        Camera camera{};
        RenderSystem renderSystem{ device,renderer.getSwapChainRenderPass() };

        auto cameraObject = GameObject::CreateGameObject();
        KeyboardController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();
        while(!window.ShouldClose())
        {
            glfwPollEvents();
            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            cameraController.MoveInPlane(window.GetGLFWWindow(), frameTime, cameraObject);
            camera.SetViewYXZ(cameraObject.transform.translation, cameraObject.transform.rotation);

            float aspect = renderer.GetAspectRatio();
            camera.SetPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 10);

            if(auto commandBuffer = renderer.BeginFrame())
            {
                renderer.BeginSwapChainRenderPass(commandBuffer);
                renderSystem.RenderGameObjects(commandBuffer, gameObjects, camera);
                renderer.EndSwapChainRenderPass(commandBuffer);
                renderer.EndFrame();
            }
        }

        vkDeviceWaitIdle(device.GetDevice());
    }

    void App::LoadGameObjects() {



        std::shared_ptr cubeModel = Model::CreateModelFromFile(device, "models/smooth_vase.obj");

        auto cube = GameObject::CreateGameObject();
        cube.model = cubeModel;
        cube.transform.translation = { 0,0,1.5 };
        cube.transform.scale = { 0.5,0.5,0.5 };
        gameObjects.push_back(std::move(cube));
    }


}
