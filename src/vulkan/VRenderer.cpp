#include "VRenderer.hpp"
#include <array>
#include <stdexcept>

VRenderer::VRenderer(VDevice &device, VSwapChain &swapChain)
    : vDevice(device), vSwapChain(swapChain) {
    recreateSwapChain();
    createCommandPool();
    createCommandBuffers();
}

VRenderer::~VRenderer() {
    vkDestroyCommandPool(vDevice.device(), commandPool, nullptr);
}

void VRenderer::recreateSwapChain() {
    auto extent = vSwapChain.getExtent();
    while (extent.width == 0 || extent.height == 0) {
        extent = vSwapChain.getExtent();
        glfwWaitEvents();
    }
    vkDeviceWaitIdle(vDevice.device());
    vSwapChain.recreate();
    vPipeline = std::make_unique<VPipeline>(
        vDevice, "core.vert", "core.frag", vSwapChain.getRenderPass());
}

void VRenderer::createCommandPool() {
    QueueFamilyIndices queueFamilyIndices =
        vDevice.findQueueFamilies(vDevice.physicalDevice());
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
    poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT |
                     VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(vDevice.device(), &poolInfo, nullptr,
                            &commandPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool!");
    }
}

void VRenderer::createCommandBuffers() {
    commandBuffers.resize(VSwapChain::MAX_FRAMES_IN_FLIGHT);
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount =
        static_cast<uint32_t>(commandBuffers.size());

    if (vkAllocateCommandBuffers(vDevice.device(), &allocInfo,
                                 commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

void VRenderer::freeCommandBuffers() {
    vkFreeCommandBuffers(vDevice.device(), commandPool,
                         static_cast<uint32_t>(commandBuffers.size()),
                         commandBuffers.data());
    commandBuffers.clear();
}


VkCommandBuffer VRenderer::beginFrame() {
    VkResult result = vSwapChain.acquireNextImage(&m_currentImageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapChain();
        return nullptr;
    }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    m_isFrameStarted = true;
    auto commandBuffer = getCurrentCommandBuffer();
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }
    return commandBuffer;
}

void VRenderer::endFrame() {
    auto commandBuffer = getCurrentCommandBuffer();
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
    auto result =
        vSwapChain.submitCommandBuffers(&commandBuffer, &m_currentImageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
        vSwapChain.framebufferResized) {
        vSwapChain.framebufferResized = false;
        recreateSwapChain();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }
    m_isFrameStarted = false;
    m_currentFrameIndex =
        (m_currentFrameIndex + 1) % VSwapChain::MAX_FRAMES_IN_FLIGHT;
}

void VRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = vSwapChain.getRenderPass();
    renderPassInfo.framebuffer = vSwapChain.getFrameBuffer(m_currentImageIndex);

    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = vSwapChain.getExtent();

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {0.0f, 0.68f, 1.0f, 1.0f};
    clearValues[1].depthStencil = {1.0f, 0};
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo,
                         VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(vSwapChain.getExtent().width);
    viewport.height = static_cast<float>(vSwapChain.getExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor{{0, 0}, vSwapChain.getExtent()};
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void VRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
    vkCmdEndRenderPass(commandBuffer);
}

void VRenderer::draw(VkCommandBuffer commandBuffer, const Primitives::Triangle &triangle) {
    vPipeline->bind(commandBuffer);

    // Push the triangle struct directly.
    vkCmdPushConstants(
        commandBuffer,
        vPipeline->getPipelineLayout(),
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
        0,
        sizeof(Primitives::Triangle),
        &triangle);

    vkCmdDraw(commandBuffer, 3, 1, 0, 0);
}
