#pragma once

#include "VDevice.hpp"
#include <vector>
#include <memory>

// Manages the Vulkan swap chain and its associated resources like images, views, framebuffers, and the render pass.
class VSwapChain {
public:
    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

    VSwapChain(VDevice &device, VkExtent2D windowExtent);
    ~VSwapChain();

    VSwapChain(const VSwapChain &) = delete;
    VSwapChain &operator=(const VSwapChain &) = delete;

    // Public members
    bool framebufferResized = false;

    // Accessors
    VkFramebuffer getFrameBuffer(int index) { return swapChainFramebuffers[index]; }
    VkRenderPass getRenderPass() { return renderPass; }
    VkExtent2D getExtent() { return swapChainExtent; }
    float extentAspectRatio() { return static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height); }
    size_t imageCount() { return swapChainImages.size(); }

    // Methods
    void recreate();
    VkResult acquireNextImage(uint32_t *pImageIndex);
    VkResult submitCommandBuffers(const VkCommandBuffer *pCommandBuffers, const uint32_t *pImageIndex);

    static SwapChainSupportDetails
    querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);

private:
    void init();
    void cleanupSwapChain();
    void createSwapChain();
    void createImageViews();
    void createRenderPass();
    void createFramebuffers();
    void createSyncObjects();

    // Helper functions for selecting optimal swap chain settings.
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

    VDevice &vDevice;
    VkExtent2D windowExtent;

    VkSwapchainKHR swapChain;
    std::shared_ptr<VSwapChain> oldSwapChain;
    VkRenderPass renderPass;

    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkFramebuffer> swapChainFramebuffers;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;

    // Sync objects
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    std::vector<VkFence> imagesInFlight;
    size_t currentFrame = 0;
};
