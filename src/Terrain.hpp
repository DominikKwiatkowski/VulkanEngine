#pragma once
#include "Model.hpp"

namespace VulkanEngine
{
    /// <summary>
    /// Random terrain generator.
    /// </summary>
    class Terrain
    {
    public:
        /// <summary>
        /// Generate random terrain based on noise
        /// </summary>
        /// <param name="device"> Current device</param>
        /// <param name="points"> number of points, total number of vertex will be points^2</param>
        /// <returns> unique_ptr<Model> generated model</returns>
        static std::unique_ptr<Model> Generate(Device& device, int points);
    };
}
