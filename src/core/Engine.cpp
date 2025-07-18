#include "Engine.hpp"
#include <array>
#include <cstring>
#include <iostream>

// Use compile-time flag to check if validation layers should be enabled.
#ifndef NDEBUG
constexpr bool enableValidationLayers = true;
#else
constexpr bool enableValidationLayers = false;
#endif

// A constexpr array is used for compile-time definition of validation layers.
constexpr std::array<const char *, 1> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};

void Engine::run() {
    init();
    mainLoop();
    cleanup();
}

void Engine::init() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // Resizing will be handled later
    window = glfwCreateWindow(WIDTH, HEIGHT, "Iro Engine", nullptr, nullptr);

    createInstance();
    createSurface();

    vDevice = std::make_unique<VDevice>(instance, surface);
    vSwapChain = std::make_unique<VSwapChain>(
        *vDevice, VkExtent2D{static_cast<uint32_t>(WIDTH),
                               static_cast<uint32_t>(HEIGHT)});

    // The pipeline no longer needs the swap chain extent, as viewport and scissor
    // are now dynamic.
    vPipeline = std::make_unique<VPipeline>(
        *vDevice, "bin/shaders/shader.vert.spv",
        "bin/shaders/shader.frag.spv", vSwapChain->getRenderPass());

    createCommandPool();
    createCommandBuffers();
    createSyncObjects();
}

void Engine::mainLoop() {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        drawFrame();
    }
    // Wait for the logical device to finish operations before cleanup.
    vkDeviceWaitIdle(vDevice->device());
}

void Engine::cleanup() {
    // Destroy semaphores and fences tied to MAX_FRAMES_IN_FLIGHT
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(vDevice->device(), imageAvailableSemaphores[i],
                           nullptr);
        vkDestroyFence(vDevice->device(), inFlightFences[i], nullptr);
    }

    // Destroy semaphores tied to swapchain image count
    for (auto semaphore : renderFinishedSemaphores) {
        vkDestroySemaphore(vDevice->device(), semaphore, nullptr);
    }


    vkDestroyCommandPool(vDevice->device(), commandPool, nullptr);

    // Smart pointers automatically handle the destruction of these objects.
    vPipeline.reset();
    vSwapChain.reset();
    vDevice.reset();

    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);

    glfwDestroyWindow(window);
    glfwTerminate();
}

void Engine::createInstance() {
    // Using designated initializers for struct setup improves clarity.
    VkApplicationInfo appInfo{
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "Iro Engine",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "Iro Engine",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = VK_API_VERSION_1_2,
    };

    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions =
        glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    VkInstanceCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &appInfo,
        .enabledExtensionCount = glfwExtensionCount,
        .ppEnabledExtensionNames = glfwExtensions,
    };

    if (enableValidationLayers) {
        createInfo.enabledLayerCount =
            static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Vulkan instance.");
    }
}

void Engine::createSurface() {
    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) !=
        VK_SUCCESS) {
        throw std::runtime_error("Failed to create window surface.");
    }
}

void Engine::createCommandPool() {
    QueueFamilyIndices queueFamilyIndices =
        vDevice->findQueueFamilies(vDevice->physicalDevice());

    VkCommandPoolCreateInfo poolInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = queueFamilyIndices.graphicsFamily.value(),
    };

    if (vkCreateCommandPool(vDevice->device(), &poolInfo, nullptr,
                            &commandPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create command pool.");
    }
}

void Engine::createCommandBuffers() {
    commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = static_cast<uint32_t>(commandBuffers.size()),
    };

    if (vkAllocateCommandBuffers(vDevice->device(), &allocInfo,
                                 commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate command buffers.");
    }
}

void Engine::createSyncObjects() {
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    
    renderFinishedSemaphores.resize(vSwapChain->imageCount());

    imagesInFlight.resize(vSwapChain->imageCount(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreInfo{
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
    VkFenceCreateInfo fenceInfo{.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
                                .flags = VK_FENCE_CREATE_SIGNALED_BIT};

    // Create sync objects tied to frames in flight
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(vDevice->device(), &semaphoreInfo, nullptr,
                              &imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(vDevice->device(), &fenceInfo, nullptr,
                          &inFlightFences[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create sync objects for a frame.");
        }
    }

    // Create sync objects tied to swapchain images
    for (size_t i = 0; i < vSwapChain->imageCount(); i++) {
        if (vkCreateSemaphore(vDevice->device(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create render finished semaphores.");
        }
    }
}

void Engine::drawFrame() {
    vkWaitForFences(vDevice->device(), 1, &inFlightFences[currentFrame], VK_TRUE,
                    UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(
        vDevice->device(), vSwapChain->getSwapChain(), UINT64_MAX,
        imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to acquire swap chain image.");
    }

    if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(vDevice->device(), 1, &imagesInFlight[imageIndex],
                        VK_TRUE, UINT64_MAX);
    }
    imagesInFlight[imageIndex] = inFlightFences[currentFrame];

    vkResetFences(vDevice->device(), 1, &inFlightFences[currentFrame]);
    vkResetCommandBuffer(commandBuffers[currentFrame], 0);

    VkCommandBufferBeginInfo beginInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    if (vkBeginCommandBuffer(commandBuffers[currentFrame], &beginInfo) !=
        VK_SUCCESS) {
        throw std::runtime_error("Failed to begin recording command buffer.");
    }

    VkClearValue clearColor = {{{0.0f, 0.68f, 1.0f, 1.0f}}};
    VkRenderPassBeginInfo renderPassInfo{
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = vSwapChain->getRenderPass(),
        .framebuffer = vSwapChain->getFrameBuffer(imageIndex),
        .renderArea = {.offset = {0, 0}, .extent = vSwapChain->getSwapChainExtent()},
        .clearValueCount = 1,
        .pClearValues = &clearColor,
    };

    vkCmdBeginRenderPass(commandBuffers[currentFrame], &renderPassInfo,
                         VK_SUBPASS_CONTENTS_INLINE);

    // Set dynamic viewport and scissor state
    VkViewport viewport{
        .x = 0.0f,
        .y = 0.0f,
        .width = static_cast<float>(vSwapChain->getSwapChainExtent().width),
        .height = static_cast<float>(vSwapChain->getSwapChainExtent().height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };
    VkRect2D scissor{.offset = {0, 0}, .extent = vSwapChain->getSwapChainExtent()};
    vkCmdSetViewport(commandBuffers[currentFrame], 0, 1, &viewport);
    vkCmdSetScissor(commandBuffers[currentFrame], 0, 1, &scissor);

    vPipeline->bind(commandBuffers[currentFrame]);
    vkCmdDraw(commandBuffers[currentFrame], 3, 1, 0, 0); // Draw hardcoded triangle
    vkCmdEndRenderPass(commandBuffers[currentFrame]);

    if (vkEndCommandBuffer(commandBuffers[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("Failed to record command buffer.");
    }

    VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
    VkPipelineStageFlags waitStages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    
    VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[imageIndex]};

    VkSubmitInfo submitInfo{
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = waitSemaphores,
        .pWaitDstStageMask = waitStages,
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBuffers[currentFrame],
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = signalSemaphores,
    };

    if (vkQueueSubmit(vDevice->graphicsQueue(), 1, &submitInfo,
                      inFlightFences[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("Failed to submit draw command buffer.");
    }

    VkSwapchainKHR swapChains[] = {vSwapChain->getSwapChain()};
    VkPresentInfoKHR presentInfo{
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = signalSemaphores,
        .swapchainCount = 1,
        .pSwapchains = swapChains,
        .pImageIndices = &imageIndex,
    };

    vkQueuePresentKHR(vDevice->presentQueue(), &presentInfo);

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}
