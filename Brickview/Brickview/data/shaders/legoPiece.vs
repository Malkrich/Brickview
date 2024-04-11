#version 330 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;

uniform mat4 u_viewProjection;
uniform mat4 u_transform;

out vec3 f_normal;

void main()
{
    f_normal = a_normal;
    gl_Position = u_viewProjection * u_transform * vec4(a_position, 1.0);
};
