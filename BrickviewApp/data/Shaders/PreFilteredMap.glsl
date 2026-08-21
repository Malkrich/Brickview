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

#include "includes/PBR/PreFilteredCreateInfoUbo.glsl"
#include "includes/PBR/Utils.glsl"

layout(location = 0) out vec3 o_color;

in vec3 f_LocalPosition;

layout(binding = 0) uniform samplerCube u_EnvironmentMap;

void main()
{
  float roughness = u_PreFilteredCreateInfo.Roughness;

  vec3 N = normalize(f_LocalPosition);
  vec3 V = N;

  const uint sampleCount = 1024;
  float totalWeight = 0.0;
  vec3 preFilteredColor = vec3(0.0);

  for (uint i = 0; i < sampleCount; i++)
  {
    vec2 Xi = Hammersley(i, sampleCount);
    vec3 H = ImportanceSampleGGX(Xi, N, roughness);
    vec3 L = reflect(-V, H);
    float NdotL = max(dot(N, L), 0.0);

    if (NdotL > 0.0)
    {
      preFilteredColor += texture(u_EnvironmentMap, L).rgb * NdotL;
      totalWeight += NdotL;
    }
  }
  preFilteredColor = preFilteredColor / totalWeight;

  o_color = preFilteredColor;
}
