#pragma once

#include "VDevice.hpp"
#include "VPipeline.hpp"
#include "VSwapChain.hpp"
#include "core/ui/Primitives.hpp"
#include <memory>
#include <vector>

// Brings together the rendering process, managing command buffers, the render pass, and drawing operations.
class VRenderer {

private:
    void createCommandPool();
    void createCommandBuffers();
    void recreateSwapChain();

    VDevice &vDevice;
    VSwapChain &vSwapChain;
    std::unique_ptr<VPipeline> vPipeline;

    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;

    uint32_t m_currentImageIndex;
    int m_currentFrameIndex = 0;
    bool m_isFrameStarted = false;


public:
    VRenderer(VDevice &device, VSwapChain &swapChain);
    ~VRenderer();

    VRenderer(const VRenderer &) = delete;
    VRenderer &operator=(const VRenderer &) = delete;

    bool isFrameInProgress() const { return m_isFrameStarted; }
    VkCommandBuffer getCurrentCommandBuffer() const;
    VkRenderPass getSwapChainRenderPass() const { return vSwapChain.getRenderPass(); }

    VkCommandBuffer beginFrame();
    void endFrame();
    void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
    void endSwapChainRenderPass(VkCommandBuffer commandBuffer);
    void draw(VkCommandBuffer commandBuffer, const Primitives::Primitive &primitive);

};
