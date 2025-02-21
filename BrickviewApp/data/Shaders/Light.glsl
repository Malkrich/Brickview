#type vertex
#version 450 core

// Per vertex
layout (location = 0) in vec3 a_position;

layout (location = 1) in int a_lightInstanceIndex;
layout (location = 2) in int a_lightInstanceEntityID;

layout (std140, binding = 0) uniform CameraData
{
    mat4 ViewProjectionMatrix;
    vec3 Position;
} cameraData;

struct PointLight
{
    vec3 Position;
    float __padding1;
    vec3 Color;
    float __padding2;
};

layout (std140, binding = 1) uniform LightsData
{
    PointLight Lights[10];
    int Count;
} lightsData;

out flat int f_lightIndex;
out flat int f_entityID;

void main()
{
    f_lightIndex = a_lightInstanceIndex;
    f_entityID = a_lightInstanceEntityID;
    vec4 worldPosition = vec4(lightsData.Lights[a_lightInstanceIndex].Position + a_position, 1.0);
    gl_Position = cameraData.ViewProjectionMatrix * worldPosition;
}

#type fragment
#version 450 core

in flat int f_lightIndex;
in flat int f_entityID;

struct PointLight
{
    vec3 Position;
    float __padding1;
    vec3 Color;
    float __padding2;
};

layout (std140, binding = 1) uniform LightsData
{
    PointLight Lights[10];
    int Count;
} lightsData;

layout (location = 0) out vec4 o_color;
layout (location = 1) out int o_entityID;

void main()
{
    o_color = vec4(lightsData.Lights[f_lightIndex].Color, 1.0);
    o_entityID = f_entityID;
}