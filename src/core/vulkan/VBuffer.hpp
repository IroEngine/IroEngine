#pragma once

#include "VDevice.hpp"

// A wrapper for a Vulkan buffer object (VkBuffer) and its associated device memory.
class VBuffer {

private:
    static VkDeviceSize getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment);

    VDevice& vDevice;
    void* mapped = nullptr;
    VkBuffer buffer = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;

    VkDeviceSize instanceSize;
    uint32_t instanceCount;
    VkBufferUsageFlags usageFlags;
    VkMemoryPropertyFlags memoryPropertyFlags;
    VkDeviceSize bufferSize;
    VkDeviceSize alignmentSize;


public:
    VBuffer(VDevice &device, VkDeviceSize instanceSize, uint32_t instanceCount, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags);
    ~VBuffer();

    VBuffer(const VBuffer &) = delete;
    VBuffer &operator=(const VBuffer &) = delete;

    VkResult map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    void unmap();

    void writeToBuffer(void *data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    VkResult flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    VkDescriptorBufferInfo descriptorInfo(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    VkResult invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

    VkBuffer getBuffer() const { return buffer; }
    void *getMappedMemory() const { return mapped; }
    uint32_t getInstanceCount() const { return instanceCount; }
    VkDeviceSize getInstanceSize() const { return instanceSize; }
    VkDeviceSize getAlignmentSize() const { return alignmentSize; }
    VkDeviceSize getBufferSize() const { return bufferSize; }

};
