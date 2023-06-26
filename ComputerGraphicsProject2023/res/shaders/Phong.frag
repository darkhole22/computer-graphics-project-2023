#version 450

layout(set = 0, binding = 1) uniform sampler2D texSampler;

layout(set = 2, binding = 0) uniform WorldBufferObject {
    vec4 pointLightPosition;
    vec4 pointLightColor;

    vec4 directLightDirection;
    vec4 directLightColor;

    vec4 cameraPosition;
} wubo;

layout(location = 0) in vec3 fragNorm;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragPos;

layout(location = 0) out vec4 outColor;

// TODO: Move this to an input which varies based on the object.
float gamma = 160.0f;

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

float pointLightDecay = 1.0f;
float pointLightG = 100.0f; // TODO Tweak this value

vec3 pointLightModel(vec3 lightColor, vec3 lightPosition, vec3 fragPosition) {
    return lightColor * pow(pointLightG / length(lightPosition - fragPos), pointLightDecay);
}

void main() {
    vec3 Norm = normalize(fragNorm);
    vec3 CameraDir = normalize(wubo.cameraPosition.xyz - fragPos);

    // Direct Light
    vec3 directLightDir = wubo.directLightDirection.xyz;
    vec3 directLightColor = wubo.directLightColor.rgb;

    vec3 DiffSpec = BRDF(CameraDir, Norm, directLightDir, texture(texSampler, fragTexCoord).rgb, vec3(1.0f), gamma);

    vec3 directLightComponent = directLightColor * DiffSpec;

    // Point Light
    vec3 pointLightDir = normalize(wubo.pointLightPosition.xyz - fragPos);
    vec3 pointLightColor = wubo.pointLightColor.rgb;

    DiffSpec = BRDF(CameraDir, Norm, pointLightDir, texture(texSampler, fragTexCoord).rgb, vec3(1.0f), gamma);
    vec3 pointLightComponent = pointLightModel(pointLightColor, wubo.pointLightPosition.xyz, fragPos) * DiffSpec;

    // Ambient Lighting
    vec3 Ambient = texture(texSampler, fragTexCoord).rgb;

    // TODO: I'm not sure why we're weighting the different components like this.
    outColor = vec4(clamp(0.95f * (directLightComponent + pointLightComponent) + 0.05f * Ambient, 0.0f, 1.0f), 1.0f);
}