#include "VRenderer.hpp"
#include "VBuffer.hpp"
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

VkCommandBuffer VRenderer::getCurrentCommandBuffer() const {
    if(!m_isFrameStarted) {
        throw std::runtime_error("Cannot get command buffer when frame not in progress");
    }

    return commandBuffers[m_currentFrameIndex];
}

void VRenderer::recreateSwapChain() {
    auto extent = vSwapChain.getExtent();
    while (extent.width == 0 || extent.height == 0) {
        glfwGetFramebufferSize(vDevice.window(), (int*)&extent.width, (int*)&extent.height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(vDevice.device());

    vSwapChain.recreate();
    vPipeline = std::make_unique<VPipeline>(vDevice, "core.vert", "core.frag", vSwapChain.getRenderPass());
}

void VRenderer::createCommandPool() {
    QueueFamilyIndices queueFamilyIndices = vDevice.findQueueFamilies(vDevice.physicalDevice());
    VkCommandPoolCreateInfo poolInfo {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
    poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(vDevice.device(), &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create command pool.");
    }
}

void VRenderer::createCommandBuffers() {
    commandBuffers.resize(VSwapChain::MAX_FRAMES_IN_FLIGHT);
    VkCommandBufferAllocateInfo allocInfo {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

    if (vkAllocateCommandBuffers(vDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate command buffers.");
    }
}

VkCommandBuffer VRenderer::beginFrame() {
    if(m_isFrameStarted) {
        throw std::runtime_error("Cannot call beginFrame while already in progress.");
    }

    auto result = vSwapChain.acquireNextImage(&m_currentImageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapChain();
        return nullptr; // Frame cannot be started.
    }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to acquire swap chain image.");
    }

    m_isFrameStarted = true;
    auto commandBuffer = getCurrentCommandBuffer();
    VkCommandBufferBeginInfo beginInfo {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin recording command buffer.");
    }

    return commandBuffer;
}

void VRenderer::endFrame() {
    if(!m_isFrameStarted) {
        throw std::runtime_error("Cannot call endFrame when frame not in progress.");
    }

    auto commandBuffer = getCurrentCommandBuffer();
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to record command buffer.");
    }

    auto result = vSwapChain.submitCommandBuffers(&commandBuffer, &m_currentImageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || vSwapChain.framebufferResized) {
        vSwapChain.framebufferResized = false;
        recreateSwapChain();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to present swap chain image.");
    }

    m_isFrameStarted = false;
    m_currentFrameIndex = (m_currentFrameIndex + 1) % VSwapChain::MAX_FRAMES_IN_FLIGHT;
}

void VRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
    if(!m_isFrameStarted) {
        throw std::runtime_error("Cannot begin render pass when frame not in progress.");
    }

    if(commandBuffer != getCurrentCommandBuffer()) {
        throw std::runtime_error("Cannot begin render pass on command buffer from a different frame.");
    }

    VkRenderPassBeginInfo renderPassInfo {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = vSwapChain.getRenderPass();
    renderPassInfo.framebuffer = vSwapChain.getFrameBuffer(m_currentImageIndex);
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = vSwapChain.getExtent();

    std::array<VkClearValue, 1> clearValues{};
    clearValues[0].color = {1.0f, 1.0f, 1.0f, 1.0f};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport {};
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
    if(!m_isFrameStarted) {
        throw std::runtime_error("Cannot end render pass when frame not in progress.");
    }

    if(commandBuffer != getCurrentCommandBuffer()) {
        throw std::runtime_error("Cannot end render pass on command buffer from a different frame.");
    }

    vkCmdEndRenderPass(commandBuffer);
}

void VRenderer::draw(VkCommandBuffer commandBuffer, const Primitives::Primitive &primitive) {
    if (primitive.getVertexCount() == 0)
        return;

    vPipeline->bind(commandBuffer);

    Primitives::PushConstantData pushData{};

    // Set transform data from the primitive.
    pushData.position = primitive.getTransform().position;
    pushData.scale = primitive.getTransform().scale;

    float aspect = vSwapChain.extentAspectRatio();
    if (aspect > 1.0f) {
        pushData.scale.x /= aspect;
    } else {
        pushData.scale.y *= aspect;
    }

    if (primitive.useBilinearInterpolation() && primitive.getVertexCount() == 4) {
        pushData.isBilinear = 1;
        // Load the four corner colors from the primitive's vertex data.
        // Assumes vertex order in C++ is BL, BR, TR, TL.
        pushData.colors[0] = primitive.getVertices()[0].color;
        pushData.colors[1] = primitive.getVertices()[1].color;
        pushData.colors[2] = primitive.getVertices()[2].color;
        pushData.colors[3] = primitive.getVertices()[3].color;
    } else {
        pushData.isBilinear = 0;
    }

    VkBuffer buffers[] = {primitive.getVertexBuffer().getBuffer()};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

    vkCmdPushConstants(commandBuffer, vPipeline->getPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(Primitives::PushConstantData), &pushData);

    if (primitive.getIndexCount() > 0) {
        vkCmdBindIndexBuffer(commandBuffer, primitive.getIndexBuffer()->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
        vkCmdDrawIndexed(commandBuffer, primitive.getIndexCount(), 1, 0, 0, 0);
    } else {
        vkCmdDraw(commandBuffer, primitive.getVertexCount(), 1, 0, 0);
    }
}
