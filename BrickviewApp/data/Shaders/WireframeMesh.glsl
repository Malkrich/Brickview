#type vertex
#version 450 core

#include "includes/Camera/CameraUbo.glsl"
#include "includes/Mesh/MeshUbo.glsl"
#include "includes/Material/WireframeMaterial.glsl"

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;

out vec3 f_Color;

void main()
{
  vec4 worldPos = u_MeshData.Transform * vec4(a_Position, 1.0);
  f_Color = u_WireframeMaterialData.Color;
  gl_Position = u_CameraData.ViewProjectionMatrix * worldPos;
}

#type fragment
#version 450 core

in vec3 f_Color;

layout(location = 0) out vec4 o_Color;

void main()
{
  o_Color = vec4(f_Color, 1.0f);
}
