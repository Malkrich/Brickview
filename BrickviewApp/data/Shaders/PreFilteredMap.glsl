#type vertex
#version 450 core

layout (location = 0) in vec3 a_position;

out vec3 f_localPosition;

layout (std140, binding = 0) uniform CubemapData
{
    mat4 ViewProjectionMatrix;
} u_cubemapData;

void main()
{
    f_localPosition = a_position;
    gl_Position = u_cubemapData.ViewProjectionMatrix * vec4(a_position, 1.0);
}

#type fragment
#version 450 core
#define PI 3.1415926538

layout (location = 0) out vec3 o_color;

in vec3 f_localPosition;

layout (std140, binding = 1) uniform PreFilteredMapData
{
    float Roughness;
} preFilteredMapData;

layout (binding = 0) uniform samplerCube u_environmentMap;

float radicalInverse_VdC(uint bits) 
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // 0x100000000
}

vec2 hammersley(uint i, uint N)
{
    return vec2(float(i)/float(N), radicalInverse_VdC(i));
}

vec3 importanceSampleGGX(vec2 Xi, vec3 N, float roughness)
{
    float a = roughness*roughness;
	
    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
	
    // from spherical coordinates to cartesian coordinates
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;
	
    // from tangent-space vector to world-space sample vector
    vec3 up        = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent   = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);
	
    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}

void main()
{
    float roughness = preFilteredMapData.Roughness;

    vec3 N = normalize(f_localPosition);
    vec3 V = N;

    const uint sampleCount = 1024;
    float totalWeight = 0.0;
    vec3 preFilteredColor = vec3(0.0);
    for (uint i = 0; i < sampleCount; i++)
    {
        vec2 Xi = hammersley(i, sampleCount);
        vec3 H = importanceSampleGGX(Xi, N, roughness);
        vec3 L = normalize(2.0 * dot(V, H) * H - V);
        float NdotL = max(dot(N, L), 0.0);
        if (NdotL > 0.0)
        {
            preFilteredColor += texture(u_environmentMap, L).rgb * NdotL;
            totalWeight += NdotL;
        }
    }
    preFilteredColor = preFilteredColor / totalWeight;

    o_color = preFilteredColor;
}
