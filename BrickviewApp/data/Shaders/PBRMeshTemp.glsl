#type vertex
#version 450 core

// Per vertex
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
// Per instance
layout (location = 2) in int a_entityID;
layout (location = 3) in vec4 a_albedo;
layout (location = 4) in mat4 a_transform;

layout (std140, binding = 0) uniform CameraData
{
    mat4 ViewProjectionMatrix;
    vec3 Position;
} cameraData;

out vec3 f_worldPosition;
out vec3 f_normal;
out vec4 f_albedo;
out flat int f_entityID;

void main()
{
    vec4 worldPosition = a_transform * vec4(a_position, 1.0);
    f_worldPosition = worldPosition.xyz;
    mat3 normalTransform = mat3(transpose(inverse(a_transform)));
    f_normal = normalTransform * a_normal;
    f_albedo = a_albedo;
    f_entityID = a_entityID;
    gl_Position = cameraData.ViewProjectionMatrix * worldPosition;
}

#type fragment
#version 450 core

#define PI           3.1415926538
#define ZERO_EPSILON 0.0000001

layout(location = 0) out vec4 o_color;
layout(location = 1) out int o_entityID;

in vec3 f_worldPosition;
in vec3 f_normal;
in vec4 f_albedo;
in flat int f_entityID;

layout (std140, binding = 0) uniform CameraData
{
    mat4 ViewProjectionMatrix;
    vec3 Position;
} cameraData;

// TODO: make array
layout (std140, binding = 1) uniform LightData
{
    vec3 Position;
    vec3 Color;
} lightData;

vec3 fresnelSchlick(vec3 baseReflectivity, vec3 viewDirection, vec3 halfwayVector)
{
    return baseReflectivity + (1.0 - baseReflectivity) * pow(1.0 - max(dot(halfwayVector, viewDirection), 0.0), 5.0);
}

float TrowbridgeReitzDistributionGGX(vec3 normal, vec3 halfwayVector, float roughness)
{
    float alpha = pow(roughness, 2.0);
    float alphaSquared = pow(alpha, 2.0);
    float normalDotHalfway = max(dot(normal, halfwayVector), 0.0);
    float denominator = pow(normalDotHalfway, 2.0) * (alphaSquared - 1.0) + 1.0;
    denominator = PI * pow(denominator, 2.0);
    return alphaSquared / max(denominator, ZERO_EPSILON);
}

float G1(vec3 normal, vec3 direction, float roughness)
{
    //float r = roughness + 1.0;
    //float k = pow(r, 2.0) / 8.0;
    float k = pow(roughness, 2.0) / 2.0;
    float normalDotDirection = max(dot(normal, direction), 0.0);
    float denominator = normalDotDirection * (1.0 - k) + k;
    return normalDotDirection / max(denominator, ZERO_EPSILON);
}

vec3 cookTorrance(vec3 viewDirection, vec3 lightDirection, vec3 normal, vec3 halfwayVector, vec3 baseReflectivity, float roughness)
{
    float D = TrowbridgeReitzDistributionGGX(normal, halfwayVector, roughness);
    float G = G1(normal, lightDirection, roughness) * G1(normal, viewDirection, roughness);
    vec3 F = fresnelSchlick(baseReflectivity, viewDirection, halfwayVector);
    vec3 numerator = D * G * F;
    float denominator = 4.0 * max(dot(normal, viewDirection), 0.0) * max(dot(normal, lightDirection), 0.0);

    denominator = max(denominator, ZERO_EPSILON);
    return numerator / denominator;
}

vec3 BRDF(vec3 albedo, vec3 lightDirection, vec3 viewDirection, vec3 normal, vec3 halfwayVector, vec3 baseReflectivity, float roughness, float metallic)
{
    vec3 specular = cookTorrance(viewDirection, lightDirection, normal, halfwayVector, baseReflectivity, roughness);
    vec3 ks = fresnelSchlick(baseReflectivity, viewDirection, halfwayVector);
    vec3 kd = vec3(1.0) - ks;
    kd *= 1.0 - metallic;

    return (kd * albedo) / PI + specular;
}

void main()
{
    float roughness = 0.0;
    float metallic = 1.0;
    vec3 albedo = f_albedo.xyz;
    vec3 baseReflectivity = mix(vec3(0.04), albedo, metallic);

    vec3 lightDirection = normalize(lightData.Position - f_worldPosition);
    vec3 viewDirection = normalize(cameraData.Position - f_worldPosition);
    vec3 halfwayVector = normalize(lightDirection + viewDirection);

    // Light
    float distance = length(lightData.Position - f_worldPosition);
    float attenuation = 1.0 / pow(distance, 2.0);
    vec3 radiance = lightData.Color * attenuation;

    vec3 brdf = BRDF(albedo, lightDirection, viewDirection, f_normal, halfwayVector, baseReflectivity, roughness, metallic);
    vec3 finalColor = brdf * radiance * max(dot(f_normal, lightDirection), 0.0);
    vec3 ambient = vec3(0.03) * albedo;

    o_color = vec4(finalColor + ambient, 1.0);
    o_entityID = f_entityID;
}