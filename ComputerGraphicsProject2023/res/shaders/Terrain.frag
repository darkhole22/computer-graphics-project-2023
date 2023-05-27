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
    float sandWidth;
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

    return oren_nayar_diffuse; // Oren-Nayar is used for materials that don't show specular reflections.
}

void main() {
    vec4 noise = texture(texSampler, fragTexCoord);

    vec3 color = step(noise.r, tbo.waterLevel) * vec3(0.196, 0.219, 0.896) +
                (1.0 - step(noise.r, tbo.waterLevel)) * step(noise.r, tbo.waterLevel + tbo.sandWidth) * vec3(0.796, 0.804, 0.4) +
                (1.0 - step(noise.r, tbo.waterLevel + tbo.sandWidth)) * vec3(0.192, 0.404, 0.0);
    
    vec3 cameraDir = normalize(wubo.cameraPosition.xyz - fragPos);
    vec3 norm = fragNorm;
    vec3 lightDir = wubo.lightDirection.xyz;
    
    //vec3 diffSpec = BRDF(cameraDir, norm, lightDir, color, vec3(1.0));
    vec3 diff = OrenNayar(cameraDir, norm, lightDir, color, 0.4);

    vec3 ambient = color * 0.05;
    
    outColor = vec4(clamp(0.95 * diff * wubo.lightColor.rgb + ambient, 0.0, 1.0), 1.0);
}