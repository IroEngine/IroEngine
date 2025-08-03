#pragma once

#include "discord/Discord.hpp"
#include "ui/UIManager.hpp"
#include "util/JobSystem.hpp"
#include "vulkan/VDevice.hpp"
#include "vulkan/VRenderer.hpp"
#include "vulkan/VSwapChain.hpp"
#include "vulkan/ThreadCommandResources.hpp"
#include <memory>
#include <vector>
#include <thread>

// Encapsulates the entire application, managing the window, core components, and the main event loop.
class Engine {

private:
    void init();
    void mainLoop();
    void cleanup();

    void createInstance();
    void createSurface();

    static void framebufferResizeCallback(GLFWwindow *window, int width, int height);

    static constexpr int INITIAL_WIDTH = 800;
    static constexpr int INITIAL_HEIGHT = 600;

    // --- Core Components ---
    GLFWwindow *window;
    VkInstance instance;
    VkSurfaceKHR surface;
    std::unique_ptr<Discord> discord;

    // --- Vulkan Abstractions ---
    std::unique_ptr<VDevice> vDevice;
    std::unique_ptr<VSwapChain> vSwapChain;
    std::unique_ptr<VRenderer> vRenderer;

    // --- UI Management ---
    std::unique_ptr<UIManager> uiManager;

    // --- Thread Management ---
    JobSystem jobSystem;
    std::array<std::vector<ThreadCommandResources>, VSwapChain::MAX_FRAMES_IN_FLIGHT> threadResources;

public:
    void run();
};
