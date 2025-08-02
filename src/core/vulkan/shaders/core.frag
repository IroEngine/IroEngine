#version 450

layout(location = 0) out vec4 outColor;

// Input from the vertex shader
layout(location = 0) in vec4 fragColor; // Default interpolated color
layout(location = 2) in vec2 inUv;      // Interpolated UV coordinate

// UPDATED: Push constant block to access the flag and corner colors
layout(push_constant, std430) uniform Push {
    vec2 position_offset;
    vec2 scale;
    uint colors[4];
    int isBilinear;
} push;

// Unpacks an 8-bit per channel RGBA color from a 32-bit unsigned integer (AABBGGRR).
vec4 uint32_aabbggrr_to_rgba(uint packed) {
    return vec4(
        (packed & 0xFF) / 255.0,
        ((packed >> 8) & 0xFF) / 255.0,
        ((packed >> 16) & 0xFF) / 255.0,
        ((packed >> 24) & 0xFF) / 255.0
    );
}

void main() {
    // If this is a quad, perform manual bilinear interpolation
    if (push.isBilinear == 1) {
        // Unpack the four corner colors sent from the CPU
        // The order matches the vertex order: BL, BR, TR, TL
        vec4 c00 = uint32_aabbggrr_to_rgba(push.colors[0]); // Bottom-Left
        vec4 c10 = uint32_aabbggrr_to_rgba(push.colors[1]); // Bottom-Right
        vec4 c11 = uint32_aabbggrr_to_rgba(push.colors[2]); // Top-Right
        vec4 c01 = uint32_aabbggrr_to_rgba(push.colors[3]); // Top-Left

        // Bilinearly interpolate the colors using the UV coordinate
        vec4 top_color = mix(c01, c11, inUv.x);
        vec4 bottom_color = mix(c00, c10, inUv.x);
        outColor = mix(bottom_color, top_color, inUv.y);
    } else {
        // Otherwise, use the default color (for the triangle)
        outColor = fragColor;
    }
}
