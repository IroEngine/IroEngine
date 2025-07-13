#version 450

// Output color variable
layout(location = 0) out vec4 outColor;

void main() {
    // Output a solid blue color with full alpha
    outColor = vec4(0.0, 0.68, 1.0, 1.0);
}
