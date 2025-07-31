#include "VBuffer.hpp"
#include <cstring>
#include <stdexcept>

VBuffer::VBuffer(VDevice &device, VkDeviceSize instanceSize, uint32_t instanceCount, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags)
    : vDevice{device}, instanceSize{instanceSize}, instanceCount{instanceCount}, usageFlags{usageFlags}, memoryPropertyFlags{memoryPropertyFlags} {

    // For uniform buffers, data must be aligned to the device's specific requirement.
    if ((usageFlags & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT) == VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT) {
        alignmentSize = getAlignment(instanceSize, vDevice.getPhysicalDeviceProperties().limits.minUniformBufferOffsetAlignment);
    } else {
        alignmentSize = instanceSize;
    }

    bufferSize = alignmentSize * instanceCount;
    vDevice.createBuffer(bufferSize, usageFlags, memoryPropertyFlags, buffer, memory);
}

VBuffer::~VBuffer() {
    unmap();
    vkDestroyBuffer(vDevice.device(), buffer, nullptr);
    vkFreeMemory(vDevice.device(), memory, nullptr);
}

VkDeviceSize VBuffer::getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment) {
    if (minOffsetAlignment > 0) {
        return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
    }

    return instanceSize;
}

VkResult VBuffer::map(VkDeviceSize size, VkDeviceSize offset) {
    return vkMapMemory(vDevice.device(), memory, offset, size, 0, &mapped);
}

void VBuffer::unmap() {
    if (mapped) {
        vkUnmapMemory(vDevice.device(), memory);
        mapped = nullptr;
    }
}

void VBuffer::writeToBuffer(void *data, VkDeviceSize size, VkDeviceSize offset) {
    if (!mapped) {
        throw std::runtime_error("Cannot write to unmapped buffer");
    }

    if (size == VK_WHOLE_SIZE) {
        memcpy(mapped, data, bufferSize);
    } else {
        char *memOffset = (char *)mapped;
        memOffset += offset;
        memcpy(memOffset, data, size);
    }
}

VkResult VBuffer::flush(VkDeviceSize size, VkDeviceSize offset) {
    VkMappedMemoryRange mappedRange {};
    mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    mappedRange.memory = memory;
    mappedRange.offset = offset;
    mappedRange.size = size;
    return vkFlushMappedMemoryRanges(vDevice.device(), 1, &mappedRange);
}

VkDescriptorBufferInfo VBuffer::descriptorInfo(VkDeviceSize size, VkDeviceSize offset) {
    return VkDescriptorBufferInfo {
        buffer,
        offset,
        size,
    };
}

VkResult VBuffer::invalidate(VkDeviceSize size, VkDeviceSize offset) {
    VkMappedMemoryRange mappedRange {};
    mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    mappedRange.memory = memory;
    mappedRange.offset = offset;
    mappedRange.size = size;
    return vkInvalidateMappedMemoryRanges(vDevice.device(), 1, &mappedRange);
}
