#version 450

layout(set = 0, binding = 0) uniform sampler2D fontAtlas;

layout(set = 0, binding = 1) uniform TextBufferObject {
    vec2 position;
    float scale;
} tbo;

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

const float width = 0.5;
const float edge = 0.1;

void main() {
    float dist = 1.0 - texture(fontAtlas, fragTexCoord).a;
    float alpha = 1.0 - smoothstep(width, width + edge, dist);
    
    outColor = vec4(1.0, 1.0, 1.0, alpha);
}
