#type vertex
#version 450 core

// Per vertex
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
// Per instance
layout (location = 2) in int a_entityID;
layout (location = 3) in vec3 a_color;
layout (location = 4) in mat4 a_transform;

layout (std140, binding = 0) uniform CameraData
{
    mat4 ViewProjectionMatrix;
    vec3 Position;
} cameraData;

out vec4 f_worldPosition;
out vec3 f_normal;
out vec3 f_color;
out flat int f_entityID;

void main()
{
    f_worldPosition = a_transform * vec4(a_position, 1.0);
    mat3 normalTransform = mat3(transpose(inverse(a_transform)));
    f_normal = normalTransform * a_normal;
    f_color = a_color;
    f_entityID = a_entityID;
    gl_Position = cameraData.ViewProjectionMatrix * f_worldPosition;
}

#type fragment
#version 450 core

layout(location = 0) out vec4 o_color;
layout(location = 1) out int o_entityID;

in vec4 f_worldPosition;
in vec3 f_normal;
in vec3 f_color;
in flat int f_entityID;

layout (std140, binding = 0) uniform CameraData
{
    mat4 ViewProjectionMatrix;
    vec3 Position;
} cameraData;

// TODO: make array
layout (std140, binding = 1) uniform LightData
{
    vec3 Position;
    vec3 Color;
} lightData;

void main()
{
    vec3 worldPosition = vec3(f_worldPosition);
    
    float ambient = 0.5;
    
    vec3 lightDirection  = normalize(lightData.Position - worldPosition);
    vec3 cameraDirection = normalize(cameraData.Position - worldPosition);
    
    float facingFactor = dot(cameraDirection, f_normal) >= 0.0 ? 1.0 : ambient;
    float diffuse = max(dot(f_normal, lightDirection), 0.0);

    float specular = 0.0;
    if(diffuse != 0.0)
    {
        float specularLight = 0.5;
        vec3 viewDirection = normalize(cameraData.Position - worldPosition);
        vec3 reflectionDirection = reflect(-lightDirection, f_normal);
        specular = pow(max(dot(viewDirection, reflectionDirection), 0.0), 64);
        specular *= specularLight;
    }

    vec3 finalColor = facingFactor * (diffuse + ambient + specular) * f_color * lightData.Color;
    o_color = vec4(finalColor, 1.0);
    o_entityID = f_entityID;
}