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

  float diffuse = max(facingFactor, 0.0);
  renderedColor *= diffuse;
  renderedColor += ambient;

  o_Color = vec4(renderedColor, 1.0);
  o_EntityID = f_EntityID;
}
