struct PointLight
{
  vec3 Position;
  vec3 Color;
};

layout(std430, binding = 1) readonly buffer LightsData
{
  uint PointLightsCount;
  PointLight PointLights[];
}
s_LightsData;
