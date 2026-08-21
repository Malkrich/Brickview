#define ZERO_EPSILON 0.0000001

struct FragmentData
{
  vec3 Position;
  vec3 Normal;
};

in FragmentData f_FragmentData;
in PBRMaterial f_Material;
in flat int f_EntityID;

layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityID;

layout(binding = 0) uniform samplerCube u_irradianceMap;
layout(binding = 1) uniform samplerCube u_preFilteredMap;
layout(binding = 2) uniform sampler2D u_brdfLUTMap;

//----------------------------------------------------------------------------
// Sources coming from: https://github.com/Nadrin/PBR/blob/master/data/shaders/glsl/pbr_fs.glsl
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// GGX/Towbridge-Reitz normal distribution function.
// Uses Disney's reparametrization of alpha = roughness^2.
float DistributionGGX(float cosLh, float roughness)
{
  float alpha = roughness * roughness;
  float alphaSq = alpha * alpha;

  float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
  return alphaSq / (PI * denom * denom);
}

//----------------------------------------------------------------------------
// Single term for separable Schlick-GGX below.
float GaSchlickG1(float cosTheta, float k)
{
  return cosTheta / (cosTheta * (1.0 - k) + k);
}

//----------------------------------------------------------------------------
// Schlick-GGX approximation of geometric attenuation function using Smith's method.
float GaSchlickGGX(float cosLi, float cosLo, float roughness)
{
  float r = roughness + 1.0;
  float k = (r * r) / 8.0; // Epic suggests using this roughness remapping for analytic lights.
  return GaSchlickG1(cosLi, k) * GaSchlickG1(cosLo, k);
}

//----------------------------------------------------------------------------
// Shlick's approximation of the Fresnel factor
vec3 FresnelSchlick(vec3 baseReflectivity, float cosTheta)
{
  return baseReflectivity + (vec3(1.0) - baseReflectivity) * pow(1.0 - cosTheta, 5.0);
}

//----------------------------------------------------------------------------
vec3 FresnelSchlickRoughness(float cosTheta, vec3 baseReflectivity, float roughness)
{
  return baseReflectivity + (max(vec3(1.0 - roughness), baseReflectivity) - baseReflectivity) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main()
{
  vec3 normal = normalize(f_FragmentData.Normal);
  vec3 viewDirection = normalize(u_CameraData.Position - f_FragmentData.Position);
  float cosLo = max(0.0, dot(normal, viewDirection));

  float roughness = f_Material.Roughness;
  float metalness = f_Material.Metalness;
  vec3 albedo = f_Material.Albedo.xyz;
  vec3 baseReflectivity = mix(vec3(0.04), albedo, metalness);

  vec3 ks = FresnelSchlickRoughness(max(dot(normal, viewDirection), 0.0), baseReflectivity, roughness);
  vec3 kd = vec3(1.0) - ks;
  kd *= 1.0 - metalness;

  // Direct lighting
  vec3 directLightingResult = vec3(0.0);
  for (uint i = 0; i < s_LightsData.PointLightsCount; i++)
  {
    vec3 lightVector = s_LightsData.PointLights[i].Position - f_FragmentData.Position;
    vec3 lightDirection = normalize(lightVector);

    float attenuation = 1.0 / pow(length(lightVector), 2.0);
    vec3 radiance = s_LightsData.PointLights[i].Color * attenuation;

    // Half-vector between light direction and view direction.
    vec3 lightViewHalfVector = normalize(lightDirection + viewDirection);

    // Calculate angles between surface normal and various light vectors.
    float cosLi = max(0.0, dot(normal, lightDirection));
    float cosLh = max(0.0, dot(normal, lightViewHalfVector));

    // Calculate Fresnel term for direct lighting.
    vec3 F = FresnelSchlick(baseReflectivity, max(0.0, dot(lightViewHalfVector, viewDirection)));
    // Calculate normal distribution for specular BRDF.
    float D = DistributionGGX(cosLh, roughness);
    // Calculate geometric attenuation for specular BRDF.
    float G = GaSchlickGGX(cosLi, cosLo, roughness);

    // Diffuse scattering happens due to light being refracted multiple times by a dielectric medium.
    // Metals on the other hand either reflect or absorb energy, so diffuse contribution is always zero.
    // To be energy conserving we must scale diffuse BRDF contribution based on Fresnel factor & metalness.
    vec3 kd = mix(vec3(1.0) - F, vec3(0.0), metalness);

    // Lambert diffuse BRDF.
    // We don't scale by 1/PI for lighting & material units to be more convenient.
    // See: https://seblagarde.wordpress.com/2012/01/08/pi-or-not-to-pi-in-game-lighting-equation/
    vec3 diffuseBRDF = kd * albedo;

    // Cook-Torrance specular microfacet BRDF.
    vec3 specularBRDF = (F * D * G) / max(ZERO_EPSILON, 4.0 * cosLi * cosLo);

    // Total contribution for this light.
    directLightingResult += (diffuseBRDF + specularBRDF) * radiance * cosLi;
  }

  // Irradiance map
  vec3 irradiance = texture(u_irradianceMap, normal).rgb;
  vec3 inderectLightingAmbient = irradiance * albedo * kd;

  // PreFiltered map and brdf LUT
  vec3 reflectViewDirection = reflect(-viewDirection, normal);

  // Make sure don't go to the last level, meaning only 1 pixel of resolution.
  // Prevents very flat shading when the material roughness is 1.0
  float maxLevelValue = max((textureQueryLevels(u_preFilteredMap) - 3.0), 0.0);
  float level = roughness * maxLevelValue;
  vec3 preFilteredColor = textureLod(u_preFilteredMap, reflectViewDirection, level).rgb;
  vec2 envBRDF = texture(u_brdfLUTMap, vec2(max(dot(normal, viewDirection), 0.0), roughness)).rg;
  vec3 inderectLightingSpecular = preFilteredColor * (ks * envBRDF.x + envBRDF.y);

  vec3 inderectLightingResult = inderectLightingAmbient + inderectLightingSpecular;

  // Final color
  vec3 finalColor = inderectLightingResult + directLightingResult;

  // Tone Mapping
  finalColor = finalColor / (finalColor + vec3(1.0));
  // Gamma correction
  finalColor = pow(finalColor, vec3(1.0 / 2.2));

  o_Color = vec4(finalColor, 1.0);
  o_EntityID = f_EntityID;
}
