#version 450

layout(set = 0, binding = 1) uniform sampler2D texSampler;

layout(set = 0, binding = 2) uniform TerrainBufferObject {
    float scale;
    float waterLevel;
    float sandWidth;
    float rockLevel;
} tbo;

layout(set = 0, binding = 3) uniform sampler2D waterSampler;
layout(set = 0, binding = 4) uniform sampler2D sandSampler;
layout(set = 0, binding = 5) uniform sampler2D grassSampler;
layout(set = 0, binding = 6) uniform sampler2D rockSampler;

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

vec3 pointLightModel(vec3 lightColor, vec3 lightPosition, vec3 fragPosition, float lightDecay, float lightMaxRange) {
    return lightColor * pow(lightMaxRange / length(lightPosition - fragPos), lightDecay);
}

// This BRDF implements OrenNayar, used for materials that do not show specular reflections
// such as our terrain.
vec3 OrenNayar(vec3 V, vec3 N, vec3 L, vec3 Md, float sigma) {
    //vec3 V  - direction of the viewer
    //vec3 N  - normal vector to the surface
    //vec3 L  - light vector (from the light model)
    //vec3 Md - main color of the surface
    //float sigma - Roughness of the model
    float A = 1 - 0.5 * pow(sigma, 2) / (pow(sigma, 2) + 0.33);
    float B = 0.45 * pow(sigma, 2) / (pow(sigma, 2) + 0.09);

    float theta_i = acos(dot(L, N));
    float theta_r = acos(dot(V, N));

    float alpha = max(theta_i, theta_r);
    float beta = min(theta_i, theta_r);

    vec3 v_i = normalize(L - dot(L, N) * N);
    vec3 v_r = normalize(V - dot(V, N) * N);
    float G = max(0, dot(v_i, v_r));

    vec3 l = Md * clamp(dot(L, N), 0.0f, 1.0f);

    vec3 oren_nayar_diffuse = l * (A + B * G * sin(alpha) * tan(beta));

    return oren_nayar_diffuse;
}

float doubleStep(float val, float minVal, float maxVal)
{
    return (1.0 - step(val, minVal)) * step(val, maxVal);
}

void main() {
    vec4 noise = texture(texSampler, fragTexCoord);

    vec3 color = step(noise.r, tbo.waterLevel) * texture(waterSampler, fragTexCoord).rgb +
                doubleStep(noise.r, tbo.waterLevel, tbo.waterLevel + tbo.sandWidth) * texture(sandSampler, fragTexCoord).rgb +
                doubleStep(noise.r, tbo.waterLevel + tbo.sandWidth, tbo.rockLevel) * texture(grassSampler, fragTexCoord).rgb +
                doubleStep(noise.r, tbo.rockLevel, 1.0) * texture(rockSampler, fragTexCoord).rgb;
    
    vec3 cameraDir = normalize(wubo.cameraPosition.xyz - fragPos);
    vec3 norm = fragNorm;

    // Direct Light
    vec3 lightDir = wubo.directLightDirection.xyz;

    vec3 diff = OrenNayar(cameraDir, norm, lightDir, color, 0.4);
    vec3 directLightComponent = wubo.directLightColor.rgb * diff;

    // Point Light
    vec3 pointLightDir = normalize(wubo.pointLightPosition.xyz - fragPos);
    vec3 pointLightColor = wubo.pointLightColor.rgb;

    diff = OrenNayar(cameraDir, norm, pointLightDir, color, 0.4);
    vec3 pointLightComponent = pointLightModel(pointLightColor, wubo.pointLightPosition.xyz, fragPos, wubo.pointLightDecay, wubo.pointLightMaxRange) * diff;

    // Ambient
    vec3 Ambient = color;

    // Out
    vec3 baseColor = clamp(0.95f * (directLightComponent + pointLightComponent) + 0.05f * Ambient, 0.0f, 1.0f);
    outColor = vec4(baseColor, 1.0);
}