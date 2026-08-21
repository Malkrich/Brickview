#type vertex
#version 450 core

#include "includes/Camera/CameraUbo.glsl"
#include "includes/Light/LightsDataSsbo.glsl"

// Per vertex
layout(location = 0) in vec3 a_Position;
// Per Instance
layout(location = 1) in int a_EntityID;

out flat int f_LightIndex;
out flat int f_EntityID;

void main()
{
  f_LightIndex = gl_InstanceID;
  f_EntityID = a_EntityID;

  vec4 worldPosition = vec4(s_LightsData.PointLights[gl_InstanceID].Position + a_Position, 1.0);
  gl_Position = u_CameraData.ViewProjectionMatrix * worldPosition;
}

#type fragment
#version 450 core

#include "includes/Light/LightsDataSsbo.glsl"

in flat int f_LightIndex;
in flat int f_EntityID;

layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityID;

void main()
{
  o_Color = vec4(s_LightsData.PointLights[f_LightIndex].Color, 1.0);
  o_EntityID = f_EntityID;
}
