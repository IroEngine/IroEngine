#pragma once

#include "core/Discord.hpp"
#include "vulkan/VDevice.hpp"
#include "vulkan/VRenderer.hpp"
#include "vulkan/VSwapChain.hpp"

#include <memory>
#include <vector>

class Engine {
private:
    // Window dimensions.
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;

    void init();
    void mainLoop();
    void cleanup();

    // Vulkan Initialization Helpers
    void createInstance();
    void createSurface();

    // --- Core Components ---
    GLFWwindow *window;
    VkInstance instance;
    VkSurfaceKHR surface;
    std::unique_ptr<Discord> discord;

    // --- Vulkan Abstractions ---
    // These smart pointers manage the lifetime of the core Vulkan components.
    std::unique_ptr<VDevice> vDevice;
    std::unique_ptr<VSwapChain> vSwapChain;
    std::unique_ptr<VRenderer> vRenderer;

    static void framebufferResizeCallback(GLFWwindow *window, int width,
                                          int height);

public:
    void run();
};
