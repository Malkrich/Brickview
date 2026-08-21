#type vertex
#version 450 core

#include "includes/Camera/CameraUbo.glsl"
#include "includes/Material/SolidMaterial.glsl"

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
out SolidMaterial f_Material;
out flat int f_EntityID;

void main()
{
  vec4 worldPosition = a_Transform * vec4(a_Position, 1.0);
  mat3 normalTransform = mat3(transpose(inverse(a_Transform)));

  f_FragmentData.Position = vec3(worldPosition);
  f_FragmentData.Normal = normalTransform * a_Normal;

  f_Material.Albedo = a_Albedo;

  f_EntityID = a_EntityID;

  gl_Position = u_CameraData.ViewProjectionMatrix * worldPosition;
}

#type fragment
#version 450 core

#include "includes/Camera/CameraUbo.glsl"
#include "includes/Material/SolidMaterial.glsl"

layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityID;

struct FragmentData
{
  vec3 Position;
  vec3 Normal;
};

in FragmentData f_FragmentData;
in SolidMaterial f_Material;
in flat int f_EntityID;

void main()
{
  float ambient = 0.15;
  vec3 renderedColor = vec3(0.6) * f_Material.Albedo.xyz;

  vec3 viewDirection = normalize(u_CameraData.Position - f_FragmentData.Position);
  float facingFactor = dot(viewDirection, f_FragmentData.Normal);
  // vec3 normalColor   = facingFactor >= 0.0 ? vec3(0.0, 0.6, 0.1) : vec3(0.8, 0.1, 0.1);

  float diffuse = max(facingFactor, 0.0);
  renderedColor *= diffuse;
  renderedColor += ambient;

  o_Color = vec4(renderedColor, 1.0);
  // color = vec4(u_showNormals ? normalColor * renderedColor: renderedColor, 1.0);
  o_EntityID = f_EntityID;
}
