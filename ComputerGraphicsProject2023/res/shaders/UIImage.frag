#version 450

layout(set = 0, binding = 0) uniform sampler2D imageSampler;

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = texture(imageSampler, fragTexCoord);
}
