#version 450

layout(set = 0, binding = 0) uniform ModelBufferObject {
    mat4 model;
} mbo;

layout(set = 0, binding = 1) uniform sampler2D noiseSampler;

layout(set = 0, binding = 2) uniform TerrainBufferObject {
    float scale;
    float waterLevel;
    float sandWidth;
    float rockLevel;
} tbo;

layout(set = 1, binding = 0) uniform CameraBufferObject {
    mat4 view;
    mat4 proj;
} cbo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNorm;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragNorm;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 fragPos;

void main() {
    // Get heightmap value
    vec4 noise = texture(noiseSampler, inTexCoord);

    // Flattens at water level
    float h0 = clamp(noise.r, tbo.waterLevel, 1.0) * tbo.scale;
    float hx = clamp(noise.g, tbo.waterLevel, 1.0) * tbo.scale;
    float hz = clamp(noise.b, tbo.waterLevel, 1.0) * tbo.scale;

    // Sets actual coord position
    vec4 position = vec4(inPosition, 1.0);
    position.y += h0;
    position = cbo.proj * cbo.view * mbo.model * position;

    gl_Position = position;

    vec3 norm = -cross(
            vec3(1.0, (hx - h0) / noise.a, 0.0),
            vec3(0.0, (hz - h0) / noise.a, 1.0) );

    fragNorm = normalize(inverse(transpose(mat3(mbo.model))) * norm);
    fragTexCoord = inTexCoord;
    fragPos = position.xyz;
}
