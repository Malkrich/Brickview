struct SolidMaterial
{
  vec4 Albedo;
};

layout(std140, binding = 3) uniform SolidMaterialData
{
  SolidMaterial Material;
}
u_SolidMaterialData;
