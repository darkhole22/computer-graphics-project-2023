#version 450

layout(set = 0, binding = 1) uniform sampler2D texSampler;
layout(set = 0, binding = 2) uniform sampler2D texEmission;
layout(set = 0, binding = 3) uniform sampler2D texRoughness;

layout(set = 0, binding = 4) uniform ObjectBufferObject {
    float emissionStrength;
} oubo;

layout(set = 2, binding = 0) uniform WorldBufferObject {
    vec4 pointLightPosition;
    vec4 pointLightColor;
    float pointLightDecay;
    float pointLightMaxRange;

    vec4 directLightDirection;
    vec4 directLightColor;

    vec4 cameraPosition;
} wubo;

layout(location = 0) in vec3 fragNorm;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragPos;

layout(location = 0) out vec4 outColor;

// This BRDF Implements Lambert Diffuse + Phong Specular
vec3 BRDF(vec3 V, vec3 N, vec3 L, vec3 Md, vec3 Ms, float gamma) {
    //vec3 V  - direction of the viewer
    //vec3 N  - normal vector to the surface
    //vec3 L  - light vector (from the light model)
    //vec3 Md - main color of the surface
    //vec3 Ms - specular color of the surface
    //float gamma - Exponent for power specular term
    vec3 lambert_diffuse = Md * max(dot(L, N), 0);

    vec3 rlx = -reflect(-L, N);
    vec3 phong_specular = Ms * pow(clamp(dot(-V, rlx), 0.0f, 1.0f), gamma);

    return lambert_diffuse + phong_specular;
}

vec3 pointLightModel(vec3 lightColor, vec3 lightPosition, vec3 fragPosition, float lightDecay, float lightMaxRange) {
    return lightColor * pow(lightMaxRange / length(lightPosition - fragPos), lightDecay);
}

void main() {
    vec3 Norm = normalize(fragNorm);
    vec3 CameraDir = normalize(wubo.cameraPosition.xyz - fragPos);
    float roughness = texture(texRoughness, fragTexCoord).r * 255.0f;

    // Direct Light
    vec3 directLightDir = wubo.directLightDirection.xyz;
    vec3 directLightColor = wubo.directLightColor.rgb;

    vec3 DiffSpec = BRDF(CameraDir, Norm, directLightDir, texture(texSampler, fragTexCoord).rgb, vec3(1.0f), roughness);
    vec3 directLightComponent = directLightColor * DiffSpec;

    // Point Light
    vec3 pointLightDir = normalize(wubo.pointLightPosition.xyz - fragPos);
    vec3 pointLightColor = wubo.pointLightColor.rgb;

    DiffSpec = BRDF(CameraDir, Norm, pointLightDir, texture(texSampler, fragTexCoord).rgb, vec3(1.0f), roughness);
    vec3 pointLightComponent = pointLightModel(pointLightColor, wubo.pointLightPosition.xyz, fragPos, wubo.pointLightDecay, wubo.pointLightMaxRange) * DiffSpec;

    // Ambient Lighting
    vec3 Ambient = texture(texSampler, fragTexCoord).rgb;

    // Emission
    vec3 Emission = texture(texEmission, fragTexCoord).rgb;
    float emissionStrength = step(1.0f, texture(texEmission, fragTexCoord).a) * oubo.emissionStrength;

    // Out
    vec3 baseColor = clamp(0.95f * (directLightComponent + pointLightComponent) + 0.05f * Ambient, 0.0f, 1.0f);
    outColor = vec4(mix(baseColor, Emission, emissionStrength), 1.0f);
}