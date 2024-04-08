# version 450

vec2 positions[6] = vec2[](
    vec2(-0.5, -0.5), // Upper left
    vec2(0.5, 0.5), // Lower right
    vec2(-0.5, 0.5), // Lower left
    vec2(-0.5, -0.5), // Upper left
    vec2(0.5, -0.5), // Upper right
    vec2(0.5, 0.5) // Lower right
);

void main() {
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
}