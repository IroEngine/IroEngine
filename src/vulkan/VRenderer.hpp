#pragma once

#include "VDevice.hpp"
#include "VPipeline.hpp"
#include "VSwapChain.hpp"
#include "VBuffer.hpp"
#include "ui/Primitives.hpp"
#include <memory>
#include <vector>

class VRenderer {
private:
    VDevice &vDevice;
    VSwapChain &vSwapChain;
    std::unique_ptr<VPipeline> vPipeline;

    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;

    uint32_t m_currentImageIndex;
    int m_currentFrameIndex = 0;
    bool m_isFrameStarted = false;

    void createCommandPool();
    void createCommandBuffers();
    void freeCommandBuffers();
    void recreateSwapChain();

public:
    VRenderer(VDevice &device, VSwapChain &swapChain);
    ~VRenderer();

    VRenderer(const VRenderer &) = delete;
    VRenderer &operator=(const VRenderer &) = delete;

    VkRenderPass getSwapChainRenderPass() const {
        return vSwapChain.getRenderPass();
    }
    float getAspectRatio() const { return vSwapChain.extentAspectRatio(); }
    bool isFrameInProgress() const { return m_isFrameStarted; }

    VkCommandBuffer getCurrentCommandBuffer() const {
        return commandBuffers[m_currentFrameIndex];
    }

    int getFrameIndex() const {
        return m_currentFrameIndex;
    }

    VkCommandBuffer beginFrame();
    void endFrame();
    void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
    void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

    void draw(VkCommandBuffer commandBuffer, const Primitives::Triangle &triangle);
};
