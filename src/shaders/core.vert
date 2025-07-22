#version 450

struct Vertex {
    vec2 position;
    uint color; // Use a uint for the packed color
};

layout(push_constant, std430) uniform Push {
    vec2  position_offset;
    vec2  scale;
    Vertex vertices[3];
} push;

layout(location = 0) out vec4 fragColor;

// Unpacks an 8-bit per channel RGBA color from a 32-bit unsigned integer.
vec4 uint32_aabbggrr_to_rgba(uint packed) {
    return vec4(
        (packed & 0xFF) / 255.0,
        ((packed >> 8) & 0xFF) / 255.0,
        ((packed >> 16) & 0xFF) / 255.0,
        ((packed >> 24) & 0xFF) / 255.0
    );
}

void main() {
    Vertex v = push.vertices[gl_VertexIndex];

    vec2 scaledPos = push.position_offset + v.position * push.scale;
    gl_Position   = vec4(scaledPos, 0.0, 1.0);
    fragColor     = uint32_aabbggrr_to_rgba(v.color); // Unpack the color for the fragment shader
}
