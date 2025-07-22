#pragma once

#include <glm/glm.hpp>
#include <cstdint>

namespace ColorUtil {

uint32_t rgba_to_uint32_aabbggrr(const glm::vec4 &color) {
    uint8_t r = static_cast<uint8_t>(color.r * 255.0f);
    uint8_t g = static_cast<uint8_t>(color.g * 255.0f);
    uint8_t b = static_cast<uint8_t>(color.b * 255.0f);
    uint8_t a = static_cast<uint8_t>(color.a * 255.0f);
    return (a << 24) | (b << 16) | (g << 8) | r;
};

}; // namespace ColorUtil
