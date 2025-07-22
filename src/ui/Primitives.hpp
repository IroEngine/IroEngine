#pragma once
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <cstdint>

namespace Primitives {

struct alignas(8) Vertex {
    glm::vec2 position;
    uint32_t color; // Packed color in 0xAABBGGRR format
};

struct alignas(16) Triangle {
    glm::vec2 position_offset{0.0f, 0.0f};
    glm::vec2 scale{1.0f, 1.0f};
    Vertex    vertices[3]{};
};

}  // namespace Primitives
