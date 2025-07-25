#pragma once

#include "VDevice.hpp"
#include <string>
#include <vector>

// Creates and manages a Vulkan graphics pipeline.
class VPipeline {
private:
    void createGraphicsPipeline(const std::string &vertShaderName,
                                const std::string &fragShaderName,
                                VkRenderPass renderPass);
    VkShaderModule createShaderModule(const std::vector<char> &code);

    VDevice &vDevice;
    VkPipeline graphicsPipeline;
    VkPipelineLayout pipelineLayout;

public:
    VPipeline(VDevice &device, const std::string &vertShaderName,
              const std::string &fragShaderName, VkRenderPass renderPass);
    ~VPipeline();

    VPipeline(const VPipeline &) = delete;
    VPipeline &operator=(const VPipeline &) = delete;

    // Binds the graphics pipeline to a command buffer.
    void bind(VkCommandBuffer commandBuffer);
    
    // Accessor for the pipeline layout
    VkPipelineLayout getPipelineLayout() const { return pipelineLayout; }
};
