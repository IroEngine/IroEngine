#pragma once
#include "Vulkan.hpp"

struct ThreadCommandResources {
    VkCommandPool   pool   {VK_NULL_HANDLE};
    VkCommandBuffer buffer {VK_NULL_HANDLE};
    bool            recorded{false};
    VkFramebuffer   framebufferUsed{VK_NULL_HANDLE};
};
