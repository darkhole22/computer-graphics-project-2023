#version 450

layout(set = 1, binding = 0) uniform ScreenBufferObject {
    float width;
    float height;
} sbo;

layout(set = 0, binding = 1) uniform TextVertexBufferObject {
    vec2 position;
    float scale;
} tbo;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec2 fragTexCoord;

void main() {
    // Scale and set pixel position
    vec2 p = inPosition * tbo.scale;
    p += tbo.position;

    // Map position in [-1, 1]
    p.x /= sbo.width / 2;
    p.y /= sbo.height / 2;
    p = p - vec2(1.0, 1.0);

    gl_Position = vec4(p, 0.5, 1.0);
    fragTexCoord = inTexCoord;
}
