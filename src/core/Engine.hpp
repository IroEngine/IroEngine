#pragma once

#include "vulkan/VDevice.hpp"
#include "vulkan/VPipeline.hpp"
#include "vulkan/VSwapChain.hpp"
#include "vulkan/Vulkan.hpp"

#include <memory>
#include <vector>

class Engine {
  private:
    // Window dimensions.
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;

    // The number of frames that can be processed concurrently.
    static constexpr size_t MAX_FRAMES_IN_FLIGHT = 2;

    void init();
    void mainLoop();
    void cleanup();

    // Vulkan Initialization Helpers
    void createInstance();
    void createSurface();
    void createCommandPool();
    void createCommandBuffers();
    void createSyncObjects();

    // Per-frame logic
    void drawFrame();

    // --- Core Components ---
    GLFWwindow *window;
    VkInstance instance;
    VkSurfaceKHR surface;
    size_t currentFrame = 0;

    // --- Vulkan Abstractions ---
    // These smart pointers manage the lifetime of the core Vulkan components.
    std::unique_ptr<VDevice> vDevice;
    std::unique_ptr<VSwapChain> vSwapChain;
    std::unique_ptr<VPipeline> vPipeline;

    // --- Command and Sync Objects ---
    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    std::vector<VkFence> imagesInFlight;

  public:
    void run();
};
