#pragma once

#include "Vulkan.hpp"
#include <vector>

// Encapsulates a Vulkan physical device (GPU) and its corresponding logical device.
class VDevice {

private:
    void pickPhysicalDevice();
    void createLogicalDevice();

    // Helpers for physical device selection
    bool isDeviceSuitable(VkPhysicalDevice device);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);

    VkPhysicalDeviceProperties properties;
    VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE;
    VkDevice device_;
    VkInstance instance_;
    VkSurfaceKHR surface_;
    GLFWwindow* window_;

    VkQueue graphicsQueue_;
    VkQueue presentQueue_;


public:
    VDevice(VkInstance instance, VkSurfaceKHR surface, GLFWwindow *window);
    ~VDevice();

    VDevice(const VDevice &) = delete;
    VDevice &operator=(const VDevice &) = delete;

    // Accessors
    VkDevice device() { return device_; }
    VkPhysicalDevice physicalDevice() { return physicalDevice_; }
    VkQueue graphicsQueue() { return graphicsQueue_; }
    VkQueue presentQueue() { return presentQueue_; }
    VkSurfaceKHR surface() { return surface_; }
    GLFWwindow *window() { return window_; }
    const VkPhysicalDeviceProperties& getPhysicalDeviceProperties() const { return properties; }

    // Utility functions
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

};
