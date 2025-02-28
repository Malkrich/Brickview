#type vertex
#version 450 core

// Per vertex
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;

layout (std140, binding = 0) uniform CameraData
{
    mat4 ViewProjectionMatrix;
    mat4 View;
    mat4 Projection;
    vec3 Position;
} u_cameraData;

layout (std140, binding = 2) uniform ModelData
{
    mat4 Transform;
    vec4 Albedo;
    float Roughness;
    float Metalness;
    int EntityID;
} u_modelData;

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
    vec4 worldPosition = u_modelData.Transform * vec4(a_position, 1.0);
    mat3 normalTransform = transpose(inverse(mat3(u_modelData.Transform)));

    // Fragment data
    f_fragmentData.Position = worldPosition.xyz;
    f_fragmentData.Normal   = normalTransform * a_normal;
    // Material
    f_material.Albedo    = u_modelData.Albedo;
    f_material.Roughness = u_modelData.Roughness;
    f_material.Metalness = u_modelData.Metalness;

    // Entity ID
    f_entityID = u_modelData.EntityID;

    // GL position
    gl_Position = u_cameraData.ViewProjectionMatrix * worldPosition;
}

#type fragment
#version 450 core

#define PI           3.1415926538
#define ZERO_EPSILON 0.0000001

layout (location = 0) out vec4 o_color;
layout (location = 1) out int o_entityID;

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
    mat4 View;
    mat4 Projection;
    vec3 Position;
} u_cameraData;

struct PointLight
{
    vec3 Position;
    vec3 Color;
};

layout (std430, binding = 1) readonly buffer LightsData
{
    uint PointLightsCount;
    PointLight PointLights[];
} s_lightsData;

vec3 fresnelSchlick(vec3 baseReflectivity, vec3 viewDirection, vec3 halfwayVector)
{
    float cosTheta = max(dot(halfwayVector, viewDirection), 0.0);
    return baseReflectivity + (1.0 - baseReflectivity) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float distributionGGX(vec3 normal, vec3 halfwayVector, float roughness)
{
    float alpha = roughness * roughness;
    float alphaSquared = alpha * alpha;
    float normalDotHalfway = max(dot(normal, halfwayVector), 0.0);
    float normalDotHalfway2 = normalDotHalfway * normalDotHalfway;
    float denominator = normalDotHalfway2 * (alphaSquared - 1.0) + 1.0;
    denominator = PI * denominator * denominator;

    float numerator = alphaSquared;

    return numerator / denominator;
}

float geometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float numerator   = NdotV;
    float denominator = NdotV * (1.0 - k) + k;
	
    return numerator / denominator;
}
float geometrySmith(vec3 normal, vec3 viewDirection, vec3 lightDirection, float roughness)
{
    float NdotL = max(dot(normal, lightDirection), 0.0);
    float NdotV = max(dot(normal, viewDirection), 0.0);
    float ggx1  = geometrySchlickGGX(NdotL, roughness);
    float ggx2  = geometrySchlickGGX(NdotV, roughness);
	
    return ggx1 * ggx2;
}

vec3 cookTorrance(vec3 viewDirection, vec3 lightDirection, vec3 normal, vec3 halfwayVector, vec3 baseReflectivity, float roughness)
{
    float D = distributionGGX(normal, halfwayVector, roughness);
    float G = geometrySmith(normal, viewDirection, lightDirection, roughness);
    vec3 F = fresnelSchlick(baseReflectivity, viewDirection, halfwayVector);
    vec3 numerator = D * G * F;
    float denominator = 4.0 * max(dot(normal, viewDirection), 0.0) * max(dot(normal, lightDirection), 0.0) + ZERO_EPSILON;

    return numerator / denominator;
}

vec3 BRDF(vec3 albedo, vec3 lightDirection, vec3 viewDirection, vec3 normal, vec3 halfwayVector, vec3 baseReflectivity, float roughness, float metalness)
{
    vec3 specular = cookTorrance(viewDirection, lightDirection, normal, halfwayVector, baseReflectivity, roughness);
    vec3 ks = fresnelSchlick(baseReflectivity, viewDirection, halfwayVector);
    vec3 kd = vec3(1.0) - ks;
    kd *= 1.0 - metalness;

    return kd * albedo / PI + specular;
}

void main()
{
    vec3 normal = normalize(f_fragmentData.Normal);
    float roughness = f_material.Roughness;
    float metalness = f_material.Metalness;
    vec3 albedo = f_material.Albedo.xyz;
    vec3 baseReflectivity = mix(vec3(0.04), albedo, metalness);

    vec3 lightResult = vec3(0.0);
    for (uint i = 0; i < s_lightsData.PointLightsCount; i++)
    {
        vec3 lightDirection = normalize(s_lightsData.PointLights[i].Position - f_fragmentData.Position);
        vec3 viewDirection = normalize(u_cameraData.Position - f_fragmentData.Position);
        vec3 halfwayVector = normalize(lightDirection + viewDirection);

        // Light
        float distance = length(s_lightsData.PointLights[i].Position - f_fragmentData.Position);
        float attenuation = 1.0 / pow(distance, 2.0);
        vec3 radiance = s_lightsData.PointLights[i].Color * attenuation;

        vec3 brdf = BRDF(albedo, lightDirection, viewDirection, normal, halfwayVector, baseReflectivity, roughness, metalness);
        lightResult += brdf * radiance * max(dot(normal, lightDirection), 0.0);
    }

    // Ambiant lighting
    vec3 ambient = vec3(0.03) * albedo;
    vec3 finalColor = ambient + lightResult;

    // Tone Mapping
    finalColor = finalColor / (finalColor + vec3(1.0));
    // Gamma correction
    finalColor = pow(finalColor, vec3(1.0/2.2));

    o_color = vec4(finalColor, 1.0);
    o_entityID = f_entityID;
}