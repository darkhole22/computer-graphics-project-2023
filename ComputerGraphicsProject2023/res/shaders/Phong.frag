#version 450

layout(set = 0, binding = 1) uniform sampler2D texSampler;

layout(set = 2, binding = 0) uniform WorldBufferObject {
    vec4 lightDirection;
    vec4 lightColor;
    vec4 cameraPosition;
} wubo;

layout(location = 0) in vec3 fragNorm;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragPos;

layout(location = 0) out vec4 outColor;

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

void main() {
    vec3 Norm = normalize(fragNorm);
    vec3 CameraDir = normalize(wubo.cameraPosition.xyz - fragPos);

    vec3 lightDir = wubo.lightDirection.xyz;
    vec3 lightColor = wubo.lightColor.rgb;

    vec3 DiffSpec = BRDF(CameraDir, Norm, lightDir, texture(texSampler, fragTexCoord).rgb, vec3(1.0f), 160.0f);
    vec3 Ambient = texture(texSampler, fragTexCoord).rgb * 0.05f;

    outColor = vec4(clamp(0.95 * (DiffSpec) * lightColor.rgb + Ambient,0.0,1.0), 1.0f);
}