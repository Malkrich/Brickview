#type vertex
#version 450

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_color;

layout (std140, binding = 0) uniform CameraData
{
    mat4 ViewProjectionMatrix;
    vec3 Position;
} cameraData;

out vec3 f_color;

void main()
{
    gl_Position = cameraData.ViewProjectionMatrix * vec4(a_position, 1.0);
    f_color = a_color;
}

#type fragment
#version 450

layout (location = 0) out vec4 o_color;

in vec3 f_color;

void main()
{
    o_color = vec4(f_color, 1.0);
}