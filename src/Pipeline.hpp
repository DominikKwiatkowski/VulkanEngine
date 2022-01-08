#pragma once

#include <string>
#include <vector>

#include "Device.hpp"

namespace VulkanEngine
{
    /// <summary>
    /// Struct to store all pipeline configure info
    /// </summary>
    struct PipelineConfigInfo {
        PipelineConfigInfo() = default;
        PipelineConfigInfo(const PipelineConfigInfo&) = delete;
        PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;


        std::vector<VkVertexInputBindingDescription> bindingDescriptions{};
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
        VkPipelineViewportStateCreateInfo viewportInfo;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        VkPipelineMultisampleStateCreateInfo multisampleInfo;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
        std::vector<VkDynamicState> dynamicStatesEnables;
        VkPipelineDynamicStateCreateInfo dynamicStateInfo;
        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        uint32_t subpass = 0;
    };

    class Pipeline
    {
    public:
        Pipeline(Device& device, const std::string& vertFilepath, const std::string& fragFilepath,
                 const PipelineConfigInfo& configInfo);
        ~Pipeline();

        Pipeline() = default;
        Pipeline(const Pipeline&) = delete;
        Pipeline &operator=(const Pipeline&) = delete;

        /// <summary>
        /// Bind pipeline to command buffer
        /// </summary>
        /// <param name="commandBuffer"> Current command buffer</param>
        void Bind(VkCommandBuffer commandBuffer);

        /// <summary>
        /// Fetch default config
        /// </summary>
        /// <param name="configInfo">reference to write to</param>
        static void DefaultPipelineConfigInfo(
            PipelineConfigInfo& configInfo);

    private:
        /// <summary>
        /// Wrapper to load byte file for shader purpose.
        /// </summary>
        /// <param name="filepath"> Path to load</param>
        /// <returns> Binary file.</returns>
        static std::vector<char> ReadFile(const std::string& filepath);

        /// <summary>
        /// Creates graphics pipeline
        /// </summary>
        /// <param name="vertFilepath"> Path to vertex shader</param>
        /// <param name="fragFilepath">Path to fragment shader</param>
        /// <param name="configInfo"> configuration of pipeline</param>
        void CreateGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath, const PipelineConfigInfo& configInfo);

        /// <summary>
        /// Create shader module from code
        /// </summary>
        /// <param name="code"> Shader code</param>
        /// <param name="pShaderModule"> Pointer to shader module to write</param>
        void CreateShaderModule(const std::vector<char>& code, VkShaderModule* pShaderModule);

        Device& device;
        VkPipeline graphicsPipeline = {};
        VkShaderModule vertShaderModule;
        VkShaderModule fragShaderModule;
    };
}
