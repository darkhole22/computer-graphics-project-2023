#version 450

layout(set = 0, binding = 0) uniform SkyboxBufferObject {
    mat4 projection;
    mat4 view;
} sbo;

layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec3 fragPos;

void main() {
    fragPos = inPosition;
    gl_Position = sbo.projection * sbo.view * vec4(inPosition, 1.0);
}
