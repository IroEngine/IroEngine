#pragma once

#include "VDevice.hpp"
#include <vector>

// Manages the Vulkan swap chain and its associated resources like
// images, views, framebuffers, and the render pass.
class VSwapChain
{
private:
    void createSwapChain();
    void createImageViews();
    void createRenderPass();
    void createFramebuffers();

    // Helper functions for selecting optimal swap chain settings.
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(
        const std::vector<VkSurfaceFormatKHR> &availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(
        const std::vector<VkPresentModeKHR> &availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

    VDevice &vDevice;
    VkExtent2D windowExtent;

    VkSwapchainKHR swapChain;
    VkRenderPass renderPass;

    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkFramebuffer> swapChainFramebuffers;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;

public:
    VSwapChain(VDevice &device, VkExtent2D windowExtent);
    ~VSwapChain();

    VSwapChain(const VSwapChain &) = delete;
    VSwapChain &operator=(const VSwapChain &) = delete;

    // Accessors
    VkFramebuffer getFrameBuffer(int index)
    {
        return swapChainFramebuffers[index];
    }
    VkRenderPass getRenderPass() { return renderPass; }
    VkSwapchainKHR getSwapChain() { return swapChain; }
    VkExtent2D getSwapChainExtent() { return swapChainExtent; }
    size_t imageCount() { return swapChainImages.size(); }

    static SwapChainSupportDetails
    querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
};
