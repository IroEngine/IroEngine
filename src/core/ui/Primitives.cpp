#include "Primitives.hpp"
#include "util/Color.hpp"
#include "core/vulkan/VBuffer.hpp"

namespace Primitives {

Primitive::~Primitive() = default;

std::vector<Vertex> Vertex::create_default_triangle() {
    return {
        {{0.0f, -0.5f}, ColorUtil::rgba_to_uint32_aabbggrr({1.0f, 1.0f, 1.0f, 1.0f})},
        {{0.5f, 0.5f},  ColorUtil::rgba_to_uint32_aabbggrr({1.0f, 1.0f, 1.0f, 1.0f})},
        {{-0.5f, 0.5f}, ColorUtil::rgba_to_uint32_aabbggrr({1.0f, 1.0f, 1.0f, 1.0f})}
    };
}

std::vector<Vertex> Vertex::create_default_quad() {
    return {
        {{-0.5f, -0.5f}, ColorUtil::rgba_to_uint32_aabbggrr({1.0f, 1.0f, 1.0f, 1.0f})},
        {{0.5f, -0.5f},  ColorUtil::rgba_to_uint32_aabbggrr({1.0f, 1.0f, 1.0f, 1.0f})},
        {{0.5f, 0.5f},   ColorUtil::rgba_to_uint32_aabbggrr({1.0f, 1.0f, 1.0f, 1.0f})},
        {{-0.5f, 0.5f},  ColorUtil::rgba_to_uint32_aabbggrr({1.0f, 1.0f, 1.0f, 1.0f})}
    };
}

std::vector<uint32_t> Quad::create_default_indices() {
    return {0, 1, 2, 2, 3, 0};
}

VkVertexInputBindingDescription Vertex::getBindingDescription() {
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 2> Vertex::getAttributeDescriptions() {
    std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
    // Position
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, position);
    // Color
    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32_UINT;
    attributeDescriptions[1].offset = offsetof(Vertex, color);
    return attributeDescriptions;
}

Primitive::Primitive(VDevice &device, const std::vector<Vertex> &initial_vertices, const std::vector<uint32_t> &initial_indices)
    : vDevice(device), vertices(initial_vertices), indices(initial_indices) {
    updateVertexBuffer();
    updateIndexBuffer();
}

void Primitive::setVertices(const std::vector<Vertex> &new_vertices) {
    this->vertices = new_vertices;
    updateVertexBuffer();
}

void Primitive::setIndices(const std::vector<uint32_t> &new_indices) {
    this->indices = new_indices;
    updateIndexBuffer();
}

void Primitive::updateVertexBuffer() {
    vertexCount = static_cast<uint32_t>(vertices.size());
    if (vertexCount == 0) {
        vertexBuffer = nullptr;
        return;
    }

    uint32_t vertexSize = sizeof(vertices[0]);
    vertexBuffer = std::make_unique<VBuffer>(
        vDevice,
        vertexSize,
        vertexCount,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );

    vertexBuffer->map();
    vertexBuffer->writeToBuffer(vertices.data());
    vertexBuffer->unmap();
}

void Primitive::updateIndexBuffer() {
    indexCount = static_cast<uint32_t>(indices.size());
    if (indexCount == 0) {
        indexBuffer = nullptr;
        return;
    }

    uint32_t indexSize = sizeof(indices[0]);
    indexBuffer = std::make_unique<VBuffer>(
        vDevice,
        indexSize,
        indexCount,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );

    indexBuffer->map();
    indexBuffer->writeToBuffer(indices.data());
    indexBuffer->unmap();
}

} // namespace Primitives
