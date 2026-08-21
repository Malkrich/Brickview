#type vertex
#version 450 core

#include "includes/Camera/CameraUbo.glsl"
#include "includes/Material/PBRMaterial.glsl"

// Per vertex
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
// Per instance
layout(location = 2) in int a_EntityID;
layout(location = 3) in vec4 a_Albedo;
layout(location = 4) in float a_Roughness;
layout(location = 5) in float a_Metalness;
layout(location = 6) in mat4 a_Transform;

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
  vec4 worldPosition = a_Transform * vec4(a_Position, 1.0);
  mat3 normalTransform = transpose(inverse(mat3(a_Transform)));

  // Fragment data
  f_FragmentData.Position = worldPosition.xyz;
  f_FragmentData.Normal = normalTransform * a_Normal;
  // Material
  f_Material.Albedo = a_Albedo;
  f_Material.Roughness = a_Roughness;
  f_Material.Metalness = a_Metalness;

  // Entity ID
  f_EntityID = a_EntityID;

  // GL position
  gl_Position = u_CameraData.ViewProjectionMatrix * worldPosition;
}

#type fragment
#version 450 core

#include "includes/Camera/CameraUbo.glsl"
#include "includes/Light/LightsDataSsbo.glsl"
#include "includes/Material/PBRMaterial.glsl"
#include "includes/PBR/Utils.glsl"

// Impementation
#include "implementations/PBRFragmentImplementation.glsl"
