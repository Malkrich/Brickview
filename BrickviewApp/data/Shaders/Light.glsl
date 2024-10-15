#type vertex
#version 450 core

layout (location = 0) in vec3 a_position;

uniform mat4 u_viewProjection;

void main()
{
    gl_Position = u_viewProjection * vec4(a_position, 1.0);
}


#type fragment
#version 450 core

layout(location = 0) out vec4 color;

uniform vec3 u_lightColor;

void main()
{
    color = vec4(u_lightColor, 1.0);
}