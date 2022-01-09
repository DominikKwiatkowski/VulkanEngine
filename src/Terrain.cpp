#pragma once
#include "Terrain.hpp"
#include <glm/gtc/noise.hpp>

namespace VulkanEngine
{
    // Function to generate a terrain. Terrain heightmap is generated using Perlin noise.
    std::unique_ptr<Model> Terrain::Generate(Device& device, int points)
    {
        VulkanEngine::Model::ModelData modelData = {};
        auto divider = 1.f / points;
        for (int i = 0; i < points; i++)
        {
            for (int j = 0; j < points; j++)
            {
                Model::Vertex vertex;
                // Calculate height map value using simplex noise.
                float height = glm::perlin(glm::vec2(i * divider * 5, j * divider * 5));
                vertex.position = glm::vec3(i * divider, height, j * divider);
                if (height < -0.6)
                    vertex.color = glm::vec3(0.764, 0.741, 0.733);
                else if (height < 0.2)
                    vertex.color = glm::vec3(0.674, 0.172, 0.066);
                else
                    vertex.color = glm::vec3(0.517, 0.756, 0.145);
                vertex.normal = glm::vec3(0.f);
                vertex.texCord = glm::vec2(0.f);
                modelData.vertices.push_back(vertex);
            }
        }

        // Calculate normals.
        for (int i = 0; i < points; i++)
        {
            for (int j = 0; j < points; j++)
            {
                Model::Vertex& vertex = modelData.vertices[i * points + j];

                // Calculate normals using the surrounding vertices.
                if (i > 0)
                {
                    vertex.normal +=
                        glm::normalize(vertex.position - modelData.vertices[(i - 1) * points + j].position);
                }
                if (i < points - 1)
                {
                    vertex.normal +=
                        glm::normalize(vertex.position - modelData.vertices[(i + 1) * points + j].position);
                }
                if (j > 0)
                {
                    vertex.normal += glm::normalize(vertex.position - modelData.vertices[i * points + j - 1].position);
                }
                if (j < points - 1)
                {
                    vertex.normal += glm::normalize(vertex.position - modelData.vertices[i * points + j + 1].position);
                }
            }
        }

        // Create index buffer.
        for (int i = 0; i < points - 1; i++)
        {
            for (int j = 0; j < points - 1; j++)
            {
                int vertexIndex = i * points + j;
                modelData.indices.push_back(vertexIndex);
                modelData.indices.push_back(vertexIndex + 1);
                modelData.indices.push_back(vertexIndex + points);
                modelData.indices.push_back(vertexIndex + 1);
                modelData.indices.push_back(vertexIndex + points + 1);
                modelData.indices.push_back(vertexIndex + points);
            }
        }

        return std::make_unique<Model>(device, modelData);
    }
}
