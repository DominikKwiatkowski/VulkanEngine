#pragma once
#include "Model.hpp"

namespace VulkanEngine
{
    class Terrain
    {
    public:
        static std::unique_ptr<Model> Generate(Device& device, int points);
    };
}
