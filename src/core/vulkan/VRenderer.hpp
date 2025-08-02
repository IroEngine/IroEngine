#pragma once

#include "ThreadCommandResources.hpp"
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

    std::array<std::vector<ThreadCommandResources>, VSwapChain::MAX_FRAMES_IN_FLIGHT> &engineThreadResources;

    uint32_t m_currentImageIndex;
    int m_currentFrameIndex = 0;
    bool m_isFrameStarted = false;


public:
    VRenderer(VDevice &device, VSwapChain &swapChain, std::array<std::vector<ThreadCommandResources>, VSwapChain::MAX_FRAMES_IN_FLIGHT> &threadRes);
    ~VRenderer();

    VRenderer(const VRenderer &) = delete;
    VRenderer &operator=(const VRenderer &) = delete;

    bool isFrameInProgress() const { return m_isFrameStarted; }
    int getFrameIndex() const { return m_currentFrameIndex; }
    VkCommandBuffer getCurrentCommandBuffer() const;
    VkFramebuffer getCurrentFramebuffer() const;
    VkRenderPass getSwapChainRenderPass() const { return vSwapChain.getRenderPass(); }

    VkCommandBuffer beginFrame();
    void endFrame();
    void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
    void endSwapChainRenderPass(VkCommandBuffer commandBuffer);
    void draw(VkCommandBuffer commandBuffer, const Primitives::Primitive &primitive);

};
