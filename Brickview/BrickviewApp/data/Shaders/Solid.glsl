#type vertex
#version 330 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in mat4 a_transform;

uniform mat4 u_viewProjection;

out vec3 f_currentPosition;
out vec3 f_normal;

void main()
{
    vec4 worldPosition = a_transform * vec4(a_position, 1.0);
    f_currentPosition = vec3(worldPosition);
    f_normal = a_normal;
    gl_Position = u_viewProjection * worldPosition;
}


#type fragment
#version 330 core

in vec3 f_currentPosition;
in vec3 f_normal;

out vec4 color;

uniform vec3 u_cameraPosition;
//uniform bool u_showNormals;

void main()
{
    float ambient = 0.15;
    vec3 renderedColor = vec3(0.6);
    
    vec3 viewDirection = normalize(u_cameraPosition - f_currentPosition);
    float facingFactor = dot(viewDirection, f_normal);
    vec3 normalColor   = facingFactor >= 0.0 ? vec3(0.0, 0.6, 0.1) : vec3(0.8, 0.1, 0.1);
    
    float diffuse = max(facingFactor, 0.0);
    renderedColor *= diffuse;
    renderedColor += ambient;

    color = vec4(renderedColor, 1.0);
    //color = vec4(u_showNormals ? normalColor * renderedColor: renderedColor, 1.0);
}