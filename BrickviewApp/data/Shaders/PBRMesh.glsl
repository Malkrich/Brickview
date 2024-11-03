#type vertex
#version 450 core

// Per vertex
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;

// Per instance
layout (location = 2) in int a_entityID;

layout (location = 3) in vec4 a_albedo;
layout (location = 4) in float a_roughness;
layout (location = 5) in float a_metalness;

layout (location = 6) in mat4 a_transform;

layout (std140, binding = 0) uniform CameraData
{
    mat4 ViewProjectionMatrix;
    vec3 Position;
} cameraData;

struct FragmentData
{
    vec3 Position;
    vec3 Normal;
};

struct Material
{
    vec4 Albedo;
    float Roughness;
    float Metalness;
};

out FragmentData f_fragmentData;
out Material f_material;
out flat int f_entityID;

void main()
{
    vec4 worldPosition = a_transform * vec4(a_position, 1.0);
    mat3 normalTransform = mat3(transpose(inverse(a_transform)));

    // Fragment data
    f_fragmentData.Position = worldPosition.xyz;
    f_fragmentData.Normal   = normalTransform * a_normal;
    // Material
    f_material.Albedo    = a_albedo;
    f_material.Roughness = a_roughness;
    f_material.Metalness = a_metalness;

    // Entity ID
    f_entityID = a_entityID;

    // GL position
    gl_Position = cameraData.ViewProjectionMatrix * worldPosition;
}

#type fragment
#version 450 core

#define PI           3.1415926538
#define ZERO_EPSILON 0.0000001

layout(location = 0) out vec4 o_color;
layout(location = 1) out int o_entityID;

struct FragmentData
{
    vec3 Position;
    vec3 Normal;
};

struct Material
{
    vec4 Albedo;
    float Roughness;
    float Metalness;
};

in FragmentData f_fragmentData;
in Material f_material;
in flat int f_entityID;

layout (std140, binding = 0) uniform CameraData
{
    mat4 ViewProjectionMatrix;
    vec3 Position;
} cameraData;

struct PointLight
{
    vec3 Position;
    vec3 Color;
};

layout (std140, binding = 1) uniform LightsData
{
    PointLight PointLights[];
    int LightCount;
} lightsData;

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
    return max(alphaSquared, ZERO_EPSILON) / max(denominator, ZERO_EPSILON);
}

float G1(vec3 normal, vec3 direction, float roughness)
{
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

vec3 BRDF(vec3 albedo, vec3 lightDirection, vec3 viewDirection, vec3 normal, vec3 halfwayVector, vec3 baseReflectivity, float roughness, float metalness)
{
    vec3 specular = cookTorrance(viewDirection, lightDirection, normal, halfwayVector, baseReflectivity, roughness);
    vec3 ks = fresnelSchlick(baseReflectivity, viewDirection, halfwayVector);
    vec3 kd = vec3(1.0) - ks;
    kd *= 1.0 - metalness;

    return (kd * albedo) / PI + specular;
}

void main()
{
    float roughness = f_material.Roughness;
    float metalness = f_material.Metalness;
    vec3 albedo = f_material.Albedo.xyz;
    vec3 baseReflectivity = mix(vec3(0.04), albedo, metalness);

    vec3 lightDirection = normalize(lightsData.Position - f_fragmentData.Position);
    vec3 viewDirection = normalize(cameraData.Position - f_fragmentData.Position);
    vec3 halfwayVector = normalize(lightDirection + viewDirection);

    // Light
    float distance = length(lightsData.Position - f_fragmentData.Position);
    float attenuation = 1.0 / pow(distance, 2.0);
    vec3 radiance = lightsData.Color * attenuation;

    vec3 brdf = BRDF(albedo, lightDirection, viewDirection, f_fragmentData.Normal, halfwayVector, baseReflectivity, roughness, metalness);
    vec3 finalColor = brdf * radiance * max(dot(f_fragmentData.Normal, lightDirection), 0.0);
    vec3 ambient = vec3(0.03) * albedo;
    finalColor += ambient;

    // Tone Mapping
    finalColor = finalColor / (finalColor + vec3(1.0));
    // Gamma correction
    finalColor = pow(finalColor, vec3(1.0/2.2));

    o_color = vec4(finalColor, 1.0);
    o_entityID = f_entityID;
}