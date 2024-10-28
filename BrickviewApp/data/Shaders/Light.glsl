#type vertex
#version 450 core

// Per vertex
layout (location = 0) in vec3 a_position;

layout (std140, binding = 0) uniform CameraData
{
    mat4 ViewProjectionMatrix;
    vec3 Position;
} cameraData;

layout (std140, binding = 2) uniform LightDrawData
{
    vec3 Position;
    vec3 Color;
    int EntityID;
} lightDrawData;

out flat int f_entityID;

void main()
{
    f_entityID = lightDrawData.EntityID;
    vec4 worldPosition = vec4(lightDrawData.Position + a_position, 1.0);
    gl_Position = cameraData.ViewProjectionMatrix * worldPosition;
}

#type fragment
#version 450 core

layout (location = 0) out vec4 o_color;
layout (location = 1) out int o_entityID;

in flat int f_entityID;

layout (std140, binding = 2) uniform LightDrawData
{
    vec3 Position;
    vec3 Color;
    int EntityID;
} lightDrawData;

void main()
{
    o_color = vec4(lightDrawData.Color, 1.0);
    o_entityID = f_entityID;
}