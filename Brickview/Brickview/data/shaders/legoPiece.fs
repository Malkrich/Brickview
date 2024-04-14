#version 330 core

in vec3 f_currentPosition;
in vec3 f_normal;
in vec3 f_color;

out vec4 color;

uniform vec3 u_lightPosition;

void main()
{
    vec3 lightPosition = vec3(3.0, 2.0, 0.0);
    vec3 lightDirection = normalize(u_lightPosition - f_currentPosition);
    
    float diffuse = max(dot(f_normal, lightDirection), 0.0);

    vec3 finalColor = diffuse * f_color;
    color = vec4(finalColor, 1.0);
    //color = vec4(0.8, 0.2, 0.2, 1.0);
};
