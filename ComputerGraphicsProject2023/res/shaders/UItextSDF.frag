#version 450

layout(set = 0, binding = 0) uniform sampler2D fontAtlas;

layout(set = 0, binding = 2) uniform TextFragmentBufferObject {
    vec4 color;
    vec4 borderColor;
    float width;
    float edge;
    float borderWidth;
    float visibility;
} tbo;

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

const float width = 0.5;

void main() {
    float dist = 1.0 - texture(fontAtlas, fragTexCoord).a;
    float alpha = 1.0 - smoothstep(tbo.width, tbo.width + tbo.edge, dist);
    float border = 1.0 - smoothstep(tbo.borderWidth, tbo.borderWidth + tbo.edge, dist) - alpha;
    
    outColor = mix(vec4(tbo.color.rgb, alpha), vec4(tbo.borderColor.rgb, border), border) * tbo.visibility;
}
