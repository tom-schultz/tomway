# version 450

layout(binding = 0) uniform Transform {
    mat4 model;
    mat4 view;
    mat4 projection;
} transform;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = transform.projection * transform.view * transform.model * vec4(inPosition, 1.0);
    float cosTheta = clamp(dot(inNormal, normalize(vec3(-1, 0.65, 0.3))), 0, 1);
    vec3 ambient_light = vec3(0.0f, 0.0f, 0.5f) * vec3(0.1f);
    fragColor = inColor * ambient_light + inColor * vec3(1.0f, 1.0f, 1.0f) * cosTheta; 
}