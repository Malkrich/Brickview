#type vertex
#version 450 core

#include "includes/Camera/CameraUbo.glsl"
#include "includes/Material/PBRMaterial.glsl"
#include "includes/Mesh/MeshUbo.glsl"

// Per vertex
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;

struct FragmentData
{
  vec3 Position;
  vec3 Normal;
};

out FragmentData f_FragmentData;
out PBRMaterial f_Material;
out flat int f_EntityID;

void main()
{
  vec4 worldPosition = u_MeshData.Transform * vec4(a_Position, 1.0);
  mat3 normalTransform = transpose(inverse(mat3(u_MeshData.Transform)));

  // Fragment data
  f_FragmentData.Position = worldPosition.xyz;
  f_FragmentData.Normal = normalTransform * a_Normal;
  // Material
  f_Material = u_PBRMaterialData.Material;

  // Entity ID
  f_EntityID = u_MeshData.EntityID;

  // GL position
  gl_Position = u_CameraData.ViewProjectionMatrix * worldPosition;
}

#type fragment
#version 450 core

#include "includes/Camera/CameraUbo.glsl"
#include "includes/Light/LightsDataSsbo.glsl"
#include "includes/Material/PBRMaterial.glsl"
#include "includes/PBR/Utils.glsl"

// Implementation
#include "implementations/PBRFragmentImplementation.glsl"
