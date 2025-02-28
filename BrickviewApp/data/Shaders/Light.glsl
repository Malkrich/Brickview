#type vertex
#version 450 core

// Per vertex
layout (location = 0) in vec3 a_position;
// Per Instance
layout (location = 1) in int a_entityID;

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

out flat int f_lightIndex;
out flat int f_entityID;

void main()
{
    f_lightIndex = gl_InstanceID;
    f_entityID = a_entityID;

    vec4 worldPosition = vec4(s_lightsData.PointLights[gl_InstanceID].Position + a_position, 1.0);
    gl_Position = u_cameraData.ViewProjectionMatrix * worldPosition;
}

#type fragment
#version 450 core

in flat int f_lightIndex;
in flat int f_entityID;

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

layout (location = 0) out vec4 o_color;
layout (location = 1) out int o_entityID;

void main()
{
    o_color = vec4(s_lightsData.PointLights[f_lightIndex].Color, 1.0);
    o_entityID = f_entityID;
}