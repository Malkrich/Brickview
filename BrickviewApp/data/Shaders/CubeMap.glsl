#type vertex
#version 450 core

layout (location = 0) in vec3 a_position;

out vec3 f_worldPosition;

layout (std140, binding = 0) uniform CameraData
{
    mat4 ViewProjectionMatrix;
    vec3 Position;
} cameraData;

void main()
{
    f_worldPosition = a_position;
    gl_Position = cameraData.ViewProjectionMatrix * vec4(a_position, 1.0);
}

#type fragment
#version 450 core

in vec3 f_worldPosition;

layout (binding = 0) uniform sampler2D u_equirectangularMap;

layout (location = 0) out vec4 o_color;

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 sampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{
    vec2 uv = sampleSphericalMap(normalize(-f_worldPosition));
    vec3 color = texture(u_equirectangularMap, uv).rgb;
    o_color = vec4(color, 1.0);
}