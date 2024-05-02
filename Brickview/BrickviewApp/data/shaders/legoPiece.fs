#version 330 core

in vec3 f_currentPosition;
in vec3 f_normal;
in vec3 f_color;

out vec4 color;

// Camera
uniform vec3 u_cameraPosition;
// Light
uniform vec3 u_lightPosition;
uniform vec3 u_lightColor;

void main()
{
    float ambient = 0.2;
    
    vec3 lightDirection = normalize(u_lightPosition - f_currentPosition);
    
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

    vec3 finalColor = (diffuse + ambient + specular) * u_lightColor * f_color;
    color = vec4(finalColor, 1.0);
};
