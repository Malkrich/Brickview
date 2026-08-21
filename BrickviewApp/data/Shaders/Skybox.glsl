#type vertex
#version 450 core

#include "includes/Camera/CameraUbo.glsl"

layout(location = 0) in vec3 a_Position;

out vec3 f_Position;

void main()
{
  f_Position = a_Position;

  // remove translation from the view matrix
  mat4 rotView = mat4(mat3(u_CameraData.View));
  vec4 clipPos = u_CameraData.Projection * rotView * vec4(a_Position, 1.0);
  gl_Position = clipPos.xyww;
}

#type fragment
#version 450 core

layout(location = 0) out vec4 o_Color;

layout(binding = 0) uniform samplerCube u_SkyboxCube;

layout(std140, binding = 1) uniform SkyboxData
{
  float MipFactor;
}
u_SkyboxData;

in vec3 f_Position;

void main()
{
  vec3 normal = normalize(f_Position);

  float level = u_SkyboxData.MipFactor * textureQueryLevels(u_SkyboxCube);
  vec3 environmentColor = textureLod(u_SkyboxCube, normal, level).rgb;

  environmentColor = environmentColor / (environmentColor + vec3(1.0));
  environmentColor = pow(environmentColor, vec3(1.0 / 2.2));

  o_Color = vec4(environmentColor, 1.0);
}
