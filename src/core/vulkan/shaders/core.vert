#version 450

// Vertex attributes from the vertex buffer
layout(location = 0) in vec2 inPosition;
layout(location = 1) in uint inColor;

// UPDATED: Push constant block matches the new C++ struct
layout(push_constant, std430) uniform Push {
    vec2 position_offset;
    vec2 scale;
    uint colors[4];
    int isBilinear;
} push;

// Output to the fragment shader
layout(location = 0) out vec4 fragColor;
// NEW: Pass a UV coordinate to the fragment shader
layout(location = 2) out vec2 outUv;

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
    vec2 finalPosition = push.position_offset + (inPosition * push.scale);
    gl_Position = vec4(finalPosition, 0.0, 1.0);
    
    // Pass the default interpolated color for non-quad objects
    fragColor = uint32_aabbggrr_to_rgba(inColor);

    // Calculate and pass UVs, mapping the [-0.5, 0.5] local space to [0, 1] texture space
    outUv = inPosition + 0.5;
}
