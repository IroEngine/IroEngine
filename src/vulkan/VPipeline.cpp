#include "VPipeline.hpp"
#include <fstream>
#include <iostream>
#include <vector>

// Reads a binary file, such as a compiled SPIR-V shader.
static std::vector<char> readFile(const std::string &filename) {
    // ate: start at end, binary: read as binary
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}

VPipeline::VPipeline(VDevice &device, const std::string &vertFilepath,
                     const std::string &fragFilepath, VkRenderPass renderPass)
    : vDevice(device) {
    createGraphicsPipeline(vertFilepath, fragFilepath, renderPass);
}

VPipeline::~VPipeline() {
    vkDestroyPipeline(vDevice.device(), graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(vDevice.device(), pipelineLayout, nullptr);
}

void VPipeline::bind(VkCommandBuffer commandBuffer) {
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      graphicsPipeline);
}

void VPipeline::createGraphicsPipeline(const std::string &vertFilepath,
                                     const std::string &fragFilepath,
                                     VkRenderPass renderPass) {
    auto vertShaderCode = readFile(vertFilepath);
    auto fragShaderCode = readFile(fragFilepath);

    VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
    VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .module = vertShaderModule,
        .pName = "main",
    };
    VkPipelineShaderStageCreateInfo fragShaderStageInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = fragShaderModule,
        .pName = "main",
    };
    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo,
                                                      fragShaderStageInfo};

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
    };
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE,
    };

    VkPipelineViewportStateCreateInfo viewportState{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .scissorCount = 1,
    };

    VkPipelineRasterizationStateCreateInfo rasterizer{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = VK_CULL_MODE_BACK_BIT,
        .frontFace = VK_FRONT_FACE_CLOCKWISE,
        .depthBiasEnable = VK_FALSE,
        .lineWidth = 1.0f,
    };

    VkPipelineMultisampleStateCreateInfo multisampling{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable = VK_FALSE,
    };

    VkPipelineColorBlendAttachmentState colorBlendAttachment{
        .blendEnable = VK_FALSE,
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                          VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    };

    VkPipelineColorBlendStateCreateInfo colorBlending{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .attachmentCount = 1,
        .pAttachments = &colorBlendAttachment,
    };

    std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT,
                                                 VK_DYNAMIC_STATE_SCISSOR};
    VkPipelineDynamicStateCreateInfo dynamicState{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
        .pDynamicStates = dynamicStates.data(),
    };

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
    };
    if (vkCreatePipelineLayout(vDevice.device(), &pipelineLayoutInfo, nullptr,
                               &pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create pipeline layout.");
    }

    VkGraphicsPipelineCreateInfo pipelineInfo{
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = 2,
        .pStages = shaderStages,
        .pVertexInputState = &vertexInputInfo,
        .pInputAssemblyState = &inputAssembly,
        .pViewportState = &viewportState,
        .pRasterizationState = &rasterizer,
        .pMultisampleState = &multisampling,
        .pColorBlendState = &colorBlending,
        .pDynamicState = &dynamicState,
        .layout = pipelineLayout,
        .renderPass = renderPass,
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE,
    };

    if (vkCreateGraphicsPipelines(vDevice.device(), VK_NULL_HANDLE, 1,
                                  &pipelineInfo, nullptr,
                                  &graphicsPipeline) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create graphics pipeline.");
    }

    vkDestroyShaderModule(vDevice.device(), fragShaderModule, nullptr);
    vkDestroyShaderModule(vDevice.device(), vertShaderModule, nullptr);
}

VkShaderModule VPipeline::createShaderModule(const std::vector<char> &code) {
    VkShaderModuleCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = code.size(),
        .pCode = reinterpret_cast<const uint32_t *>(code.data()),
    };

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(vDevice.device(), &createInfo, nullptr,
                             &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create shader module.");
    }
    return shaderModule;
}
