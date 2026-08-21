#type vertex
#version 450 core

#include "includes/Camera/CubemapCameraUbo.glsl"

layout(location = 0) in vec3 a_Position;

out vec3 f_LocalPosition;

void main()
{
  f_LocalPosition = a_Position;
  gl_Position = u_CubemapCamera.ViewProjectionMatrix * vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec3 o_Color;

in vec3 f_LocalPosition;

layout(binding = 0) uniform sampler2D u_EquirectangularTexture;

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
  vec3 normal = normalize(-f_LocalPosition);

  vec2 uv = sampleSphericalMap(normal);
  o_Color = texture(u_EquirectangularTexture, uv).rgb;
}
