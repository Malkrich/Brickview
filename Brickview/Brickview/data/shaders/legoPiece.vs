#version 330 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec3 a_color;

uniform mat4 u_viewProjection;

out vec3 f_currentPosition;
out vec3 f_normal;
out vec3 f_color;

void main()
{
    f_currentPosition = a_position;
    f_normal = a_normal;
    f_color = a_color;
    gl_Position = u_viewProjection * vec4(f_currentPosition, 1.0);
};
