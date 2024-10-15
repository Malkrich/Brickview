#type vertex
#version 450 core

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
}

#type fragment
#version 450 core

in vec3 f_currentPosition;
in vec3 f_normal;
in vec3 f_color;

layout(location = 0) out vec4 color;

// Camera
uniform vec3 u_cameraPosition;
// Light
uniform vec3 u_lightPosition;
uniform vec3 u_lightColor;

void main()
{
    float ambient = 0.2;
    
    vec3 lightDirection  = normalize(u_lightPosition - f_currentPosition);
    vec3 cameraDirection = normalize(u_cameraPosition - f_currentPosition);
    
    float facingFactor = dot(cameraDirection, f_normal) >= 0.0 ? 1.0 : ambient;
    float diffuse = max(dot(f_normal, lightDirection), 0.0);

    float specular = 0.0;
    if(diffuse != 0.0)
    {
        float specularLight = 0.5;
        vec3 viewDirection = normalize(u_cameraPosition - f_currentPosition);
        vec3 reflectionDirection = reflect(-lightDirection, f_normal);
        specular = pow(max(dot(viewDirection, reflectionDirection), 0.0), 64);
        specular *= specularLight;
    }

    vec3 finalColor = facingFactor * (diffuse + ambient + specular) * u_lightColor * f_color;
    color = vec4(finalColor, 1.0);
}