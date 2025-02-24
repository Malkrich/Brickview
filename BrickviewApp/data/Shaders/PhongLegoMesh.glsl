#type vertex
#version 450 core

// Per vertex
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
// Per instance
layout (location = 2) in int a_entityID;
layout (location = 3) in vec4 a_albedo;
layout (location = 4) in mat4 a_transform;

layout (std140, binding = 0) uniform CameraData
{
    mat4 ViewProjectionMatrix;
    vec3 Position;
} cameraData;

out vec4 f_worldPosition;
out vec3 f_normal;
out vec4 f_albedo;
out flat int f_entityID;

void main()
{
    f_worldPosition = a_transform * vec4(a_position, 1.0);
    mat3 normalTransform = mat3(transpose(inverse(a_transform)));
    f_normal = normalTransform * a_normal;
    f_albedo = a_albedo;
    f_entityID = a_entityID;
    gl_Position = cameraData.ViewProjectionMatrix * f_worldPosition;
}

#type fragment
#version 450 core

layout (location = 0) out vec4 o_color;
layout (location = 1) out int o_entityID;

in vec4 f_worldPosition;
in vec3 f_normal;
in vec4 f_albedo;
in flat int f_entityID;

layout (std140, binding = 0) uniform CameraData
{
    mat4 ViewProjectionMatrix;
    vec3 Position;
} cameraData;

struct PointLight
{
    vec3 Position;
    float __padding1;
    vec3 Color;
    float __padding2;
};

layout (std140, binding = 1) uniform LightsData
{
    int LightCount;
    PointLight PointLights[10];
} lightsData;

void main()
{
    vec3 worldPosition = vec3(f_worldPosition);
    
    float ambient = 0.3;
    
    vec3 lightDirection  = normalize(lightsData.PointLights[0].Position - worldPosition);
    vec3 cameraDirection = normalize(cameraData.Position - worldPosition);
    
    float diffuse = max(dot(f_normal, lightDirection), 0.0);

    float specular = 0.0;
    if(diffuse != 0.0)
    {
        float specularLight = 0.5;
        vec3 viewDirection = normalize(cameraData.Position - worldPosition);
        vec3 reflectionDirection = reflect(-lightDirection, f_normal);
        specular = pow(max(dot(viewDirection, reflectionDirection), 0.0), 128);
        specular *= specularLight;
    }

    o_color = vec4((diffuse + ambient + specular) * f_albedo.xyz * lightsData.PointLights[0].Color, 1.0);
    o_color = vec4(1.0f, 1.0f, 0.0f, 1.0f);
    o_entityID = f_entityID;
}