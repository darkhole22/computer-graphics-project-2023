#version 450

layout(set = 0, binding = 1) uniform sampler2D texSampler;

layout(set = 2, binding = 0) uniform WorldBufferObject {
    vec4 lightDirection;
    vec4 lightColor;
    vec4 cameraPosition;
} wubo;

layout(set = 0, binding = 2) uniform TerrainBufferObject {
    float scale;
    float waterLevel;
} tbo;

layout(location = 0) in vec3 fragNorm;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragPos;

layout(location = 0) out vec4 outColor;

// TODO: Move this to an input which varies based on the object.
float gamma = 160.0;

vec3 BRDF(vec3 V, vec3 N, vec3 L, vec3 Md, vec3 Ms) {
    //vec3 V  - direction of the viewer
    //vec3 N  - normal vector to the surface
    //vec3 L  - light vector (from the light model)
    //vec3 Md - main color of the surface
    //vec3 Ms - specular color of the surface
    
    //float gamma - Exponent for power specular term
    vec3 lambert_diffuse = Md * max(dot(L, N), 0);

    vec3 rlx = -reflect(-L, N);
    vec3 phong_specular = Ms * pow(clamp(dot(-V, rlx), 0.0, 1.0), gamma);

    return lambert_diffuse + phong_specular;
}

const float sandWidth = 0.05;

void main() {
    vec4 noise = texture(texSampler, fragTexCoord);

    vec3 color = step(noise.r, tbo.waterLevel) * vec3(0.196, 0.219, 0.896) +
                (1.0 - step(noise.r, tbo.waterLevel)) * step(noise.r, tbo.waterLevel + sandWidth) * vec3(0.796, 0.804, 0.4) +
                (1.0 - step(noise.r, tbo.waterLevel + sandWidth)) * vec3(0.192, 0.404, 0.0);
    
    vec3 cameraDir = normalize(wubo.cameraPosition.xyz - fragPos);
    vec3 norm = fragNorm;
    vec3 lightDir = wubo.lightDirection.xyz;
    
    vec3 diffSpec = BRDF(cameraDir, norm, lightDir, color, vec3(1.0));
    vec3 ambient = color * 0.05;
    
    outColor = vec4(clamp(0.95 * diffSpec * wubo.lightColor.rgb + ambient, 0.0, 1.0), 1.0);
}