#pragma once

#include "VDevice.hpp"
#include <string>
#include <vector>

// Creates and manages a Vulkan graphics pipeline, including shader loading, vertex input descriptions, and pipeline state configuration.
class VPipeline {

private:
    void createGraphicsPipeline(const std::string &vertShaderName, const std::string &fragShaderName, VkRenderPass renderPass);
    VkShaderModule createShaderModule(const std::vector<char> &code);

    VDevice &vDevice;
    VkPipeline graphicsPipeline;
    VkPipelineLayout pipelineLayout;


public:
    VPipeline(VDevice &device, const std::string &vertShaderName, const std::string &fragShaderName, VkRenderPass renderPass);
    ~VPipeline();

    VPipeline(const VPipeline &) = delete;
    VPipeline &operator=(const VPipeline &) = delete;

    void bind(VkCommandBuffer commandBuffer);
    VkPipelineLayout getPipelineLayout() const { return pipelineLayout; }

};
