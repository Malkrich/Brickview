#version 330 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec3 a_color;

uniform mat4 u_viewProjection;
uniform mat4 u_transform;

out vec3 f_currentPosition;
out vec3 f_normal;

void main()
{
    f_currentPosition = vec3(u_transform * vec4(a_position, 1.0));
    f_normal = a_normal;
    gl_Position = u_viewProjection * vec4(f_currentPosition, 1.0);
};
