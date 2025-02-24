#type vertex
#version 450 core

layout (location = 0) in vec3 a_position;

layout (std140, binding = 0) uniform CameraData
{
    mat4 ViewProjectionMatrix;
    vec3 Position;
} cameraData;

layout (std140, binding = 2) uniform ModelData
{
    mat4 Transform;
    vec4 Albedo;
    float Roughness;
    float Metalness;
    int EntityID;
} modelData;

void main()
{
    vec4 worldPos = modelData.Transform * vec4(a_position, 1.0);
    gl_Position = cameraData.ViewProjectionMatrix * worldPos;
}

#type fragment
#version 450 core

layout (location = 0) out vec4 o_color;

void main()
{
    o_color = vec4(0.0f, 1.0f, 0.0f, 1.0f);
}