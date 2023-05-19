#version 450

layout(set = 1, binding = 0) uniform CameraBufferObject {
    mat4 view;
    mat4 proj;
} cbo;

layout(set = 0, binding = 0) uniform ModelBufferObject {
    mat4 model;
} mbo;

layout(set = 0, binding = 1) uniform sampler2D noiseSampler;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNorm;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragNorm;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 fragPos;

void main() {
    vec4 noise = texture(noiseSampler, inTexCoord);
    vec4 position = vec4(inPosition, 1.0);
    position.y += noise.a * 15.0;

    position = cbo.proj * cbo.view * mbo.model * position;

    gl_Position = position;

    fragNorm = inverse(transpose(mat3(mbo.model))) * inNorm;
    fragTexCoord = inTexCoord;
    fragPos = position.xyz;
}
