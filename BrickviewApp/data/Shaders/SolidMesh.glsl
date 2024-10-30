#type vertex
#version 450 core

// Per vertices1
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
// Per instances
layout (location = 2) in int a_entityID;
layout (location = 3) in vec4 a_albedo;
layout (location = 4) in mat4 a_transform;

layout (std140, binding = 0) uniform CameraData
{
    mat4 ViewProjectionMatrix;
    vec3 Position;
} cameraData;

out vec3 f_currentPosition;
out vec3 f_normal;
out vec4 f_albedo;
out flat int f_entityID;

void main()
{
    vec4 worldPosition = a_transform * vec4(a_position, 1.0);
    f_currentPosition = vec3(worldPosition);
    mat3 normalTransform = mat3(transpose(inverse(a_transform)));
    f_normal = normalTransform * a_normal;
    f_albedo = a_albedo;
    f_entityID = a_entityID;
    gl_Position = cameraData.ViewProjectionMatrix * worldPosition;
}


#type fragment
#version 450 core

layout (location = 0) out vec4 o_color;
layout (location = 1) out int o_entityID;

in vec3 f_currentPosition;
in vec3 f_normal;
in vec4 f_albedo;
in flat int f_entityID;

layout (std140, binding = 0) uniform CameraData
{
    mat4 ViewProjectionMatrix;
    vec3 Position;
} cameraData;
//uniform bool u_showNormals;

void main()
{
    float ambient = 0.15;
    vec3 renderedColor = vec3(0.6) * f_albedo.xyz;
    
    vec3 viewDirection = normalize(cameraData.Position - f_currentPosition);
    float facingFactor = dot(viewDirection, f_normal);
    //vec3 normalColor   = facingFactor >= 0.0 ? vec3(0.0, 0.6, 0.1) : vec3(0.8, 0.1, 0.1);
    
    float diffuse = max(facingFactor, 0.0);
    renderedColor *= diffuse;
    renderedColor += ambient;

    o_color = vec4(renderedColor, 1.0);
    //color = vec4(u_showNormals ? normalColor * renderedColor: renderedColor, 1.0);
    o_entityID = f_entityID;
}