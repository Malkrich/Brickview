#version 330 core

in vec3 f_currentPosition;
in vec3 f_normal;

out vec4 color;

uniform vec3 u_cameraPosition;

void main()
{
    float ambient = 0.2;
    vec3 renderedColor = vec3(0.6);
    
    vec3 viewDirection = normalize(u_cameraPosition - f_currentPosition);
    float diffuse = max(dot(viewDirection, f_normal), 0.0);
    renderedColor *= diffuse;
    renderedColor += ambient;

    color = vec4(renderedColor, 1.0);
};
