struct PBRMaterial
{
  vec4 Albedo;
  float Roughness;
  float Metalness;
};

layout(std140, binding = 3) uniform PBRMaterialData
{
  PBRMaterial Material;
}
u_PBRMaterialData;
