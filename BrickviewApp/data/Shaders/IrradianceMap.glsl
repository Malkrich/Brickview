#type vertex
#version 450 core

#include "includes/Camera/CubemapCameraUbo.glsl"

layout(location = 0) in vec3 a_position;

out vec3 f_localPosition;


void main()
{
  f_localPosition = a_position;
  gl_Position = u_CubemapCamera.ViewProjectionMatrix * vec4(f_localPosition, 1.0);
}

#type fragment
#version 450 core

#include "includes/PBR/EnvironmentCreateInfoUbo.glsl"
#include "includes/PBR/Utils.glsl"

layout(location = 0) out vec3 o_color;

in vec3 f_localPosition;

layout(binding = 0) uniform samplerCube u_environmentMap;


void main()
{
  // normal
  vec3 N = normalize(f_localPosition);

  vec3 up = vec3(0.0, 1.0, 0.0);
  vec3 right = normalize(cross(up, N));
  up = normalize(cross(N, right));

  const float roughness = 1.0;
  const uint sampleCount = 16384;

  float totalWeight = 0.0;
  vec3 irradiance = vec3(0.0);
  for (uint i = 0u; i < sampleCount; ++i)
  {
    vec2 Xi = Hammersley(i, sampleCount);
    vec3 H = ImportanceSampleGGX(Xi, N, roughness);

    // NdotH is equal to cos(theta)
    float NdotH = max(dot(N, H), 0.0);
    // With roughness == 1 in the distribution function we get 1/pi
    float D = roughness / PI;
    float pdf = (D * NdotH / 4.0) + 0.0001;

    float resolution = float(u_EnvironmentCreateInfo.EnvironmentMapDimensions); // resolution of source cubemap (per face)
    // with a higher resolution, we should sample coarser mipmap levels
    float saTexel = 4.0 * PI / (6.0 * resolution * resolution);
    // as we take more samples, we can sample from a finer mipmap.
    // And places where H is more likely to be sampled (higher pdf) we
    // can use a finer mipmap, otherwise use courser mipmap.
    // The tutorial treats this portion as optional to reduce noise but I think it's
    // actually necessary for importance sampling to get the correct result
    float saSample = 1.0 / (float(sampleCount) * pdf + 0.0001);

    float mipLevel = 0.5 * log2(saSample / saTexel);

    irradiance += textureLod(u_environmentMap, H, mipLevel).rgb * NdotH;
    totalWeight += NdotH;
  }

  irradiance = (PI * irradiance) / totalWeight;
  o_color = irradiance;
}
