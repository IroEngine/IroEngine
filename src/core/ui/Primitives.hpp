#pragma once

#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include "core/vulkan/VDevice.hpp"
#include <cstdint>
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <array>

class VBuffer;

namespace Primitives {

struct alignas(16) PushConstantData {
    glm::vec2 position{0.0f, 0.0f};
    glm::vec2 scale{1.0f, 1.0f};
    uint32_t colors[4];
    int isBilinear;
};

// Represents a single vertex with 2D position and a packed 32-bit color.
struct alignas(8) Vertex {
    glm::vec2 position;
    uint32_t color; // 0xAABBGGRR format

    static std::vector<Vertex> create_default_triangle();
    static std::vector<Vertex> create_default_quad();

    static VkVertexInputBindingDescription getBindingDescription();
    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();
};

// Data structure for 2D transformations, passed to shaders via push constants.
struct alignas(16) Transform {
    glm::vec2 position{0.0f, 0.0f};
    glm::vec2 scale{1.0f, 1.0f};
};

// Base class for all drawable geometric shapes.
class Primitive {

protected:
    void updateVertexBuffer();
    void updateIndexBuffer();

    VDevice &vDevice;
    Transform transform{};
    std::vector<Vertex> vertices;
    std::unique_ptr<VBuffer> vertexBuffer;
    uint32_t vertexCount = 0;

    std::vector<uint32_t> indices;
    std::unique_ptr<VBuffer> indexBuffer;
    uint32_t indexCount = 0;

    bool dirty_ = true;


public:
    Primitive(VDevice &device, const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices = {});
    virtual ~Primitive();

    virtual bool useBilinearInterpolation() const { return false; }

    void setPosition(const glm::vec2 &pos) { transform.position = pos; }
    void setScale(const glm::vec2 &scl) { transform.scale = scl; }
    void setVertices(const std::vector<Vertex> &vertices);
    void setIndices(const std::vector<uint32_t> &indices);

    bool dirty() const { return dirty_; }
    void clearDirty() { dirty_ = false; }

    const VBuffer &getVertexBuffer() const { return *vertexBuffer; }
    uint32_t getVertexCount() const { return vertexCount; }
    const VBuffer *getIndexBuffer() const { return indexBuffer.get(); }
    uint32_t getIndexCount() const { return indexCount; }
    const Transform &getTransform() const { return transform; }
    const std::vector<Vertex>& getVertices() const { return vertices; }

};

class Triangle : public Primitive {
public:
    Triangle(VDevice &device) : Primitive(device, Vertex::create_default_triangle()) {}
    Triangle(VDevice &device, const std::vector<Vertex> &vertices) : Primitive(device, vertices) {}
};

class Quad : public Primitive {
public:
    static std::vector<uint32_t> create_default_indices();
    Quad(VDevice &device) : Primitive(device, Vertex::create_default_quad(), create_default_indices()) {}
    Quad(VDevice &device, const std::vector<Vertex> &vertices) : Primitive(device, vertices, create_default_indices()) {}

    // NEW: Override the virtual function to return true for Quads.
    bool useBilinearInterpolation() const override { return true; }
};

} // namespace Primitives
