#type vertex
#version 450

#include "includes/Camera/CameraUbo.glsl"

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Color;

out vec3 f_Color;

void main()
{
    gl_Position = u_CameraData.ViewProjectionMatrix * vec4(a_Position, 1.0);
    f_Color = a_Color;
}

#type fragment
#version 450

layout (location = 0) out vec4 o_Color;

in vec3 f_Color;

void main()
{
    o_Color = vec4(f_Color, 1.0);
}
