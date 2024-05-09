#type vertex
#version 330 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;

uniform mat4 u_viewProjection;

out vec3 f_currentPosition;
out vec3 f_normal;

void main()
{
    f_currentPosition = a_position;
    f_normal = a_normal;
    gl_Position = u_viewProjection * vec4(f_currentPosition, 1.0);
}


#type fragment
#version 330 core

in vec3 f_currentPosition;
in vec3 f_normal;

out vec4 color;

uniform vec3 u_cameraPosition;

void main()
{
    float ambient = 0.15;
    vec3 renderedColor = vec3(0.6);
    
    vec3 viewDirection = normalize(u_cameraPosition - f_currentPosition);
    float diffuse = max(dot(viewDirection, f_normal), 0.0);
    renderedColor *= diffuse;
    renderedColor += ambient;

    color = vec4(renderedColor, 1.0);
}