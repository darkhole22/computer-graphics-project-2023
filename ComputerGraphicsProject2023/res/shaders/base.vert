#version 450

layout(set = 1, binding = 0) uniform CameraBufferObject {
    mat4 view;
    mat4 proj;
} cbo;

layout(set = 0, binding = 0) uniform ModelBufferObject {
    mat4 model;
} mbo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNorm;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragNorm;
layout(location = 1) out vec2 fragTexCoord;

void main() {
    gl_Position = cbo.proj * cbo.view * mbo.model * vec4(inPosition, 1.0);
    fragNorm = inNorm;
    fragTexCoord = inTexCoord;
}
