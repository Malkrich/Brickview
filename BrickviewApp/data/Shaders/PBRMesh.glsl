#type vertex
#version 450 core

// Per vertex
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
// Per instance
layout (location = 2) in int a_entityID;
layout (location = 3) in vec4 a_albedo;
layout (location = 4) in mat4 a_transform;

layout (std140, binding = 0) uniform CameraData
{
    mat4 ViewProjectionMatrix;
    vec3 Position;
} cameraData;

out vec3 f_worldPosition;
out vec3 f_normal;
out vec4 f_albedo;
out flat int f_entityID;

void main()
{
    vec4 worldPosition = a_transform * vec4(a_position, 1.0);
    f_worldPosition = worldPosition.xyz;
    mat3 normalTransform = mat3(transpose(inverse(a_transform)));
    f_normal = normalTransform * a_normal;
    f_albedo = a_albedo;
    f_entityID = a_entityID;
    gl_Position = cameraData.ViewProjectionMatrix * worldPosition;
}

#type fragment
#version 450 core

#define PI           3.1415926538
#define ZERO_EPSILON 0.0000001

layout(location = 0) out vec4 o_color;
layout(location = 1) out int o_entityID;

in vec3 f_worldPosition;
in vec3 f_normal;
in vec4 f_albedo;
in flat int f_entityID;

layout (std140, binding = 0) uniform CameraData
{
    mat4 ViewProjectionMatrix;
    vec3 Position;
} cameraData;

// TODO: make array
layout (std140, binding = 1) uniform LightData
{
    vec3 Position;
    vec3 Color;
} lightData;

// GGX/Towbridge-Reitz normal distribution function.
// Uses Disney's reparametrization of alpha = roughness^2.
float ndfGGX(float cosLh, float roughness)
{
	float alpha   = roughness * roughness;
	float alphaSq = alpha * alpha;

	float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
	return alphaSq / (PI * denom * denom);
}

// Single term for separable Schlick-GGX below.
float gaSchlickG1(float cosTheta, float k)
{
	return cosTheta / (cosTheta * (1.0 - k) + k);
}

// Schlick-GGX approximation of geometric attenuation function using Smith's method.
float gaSchlickGGX(float cosLi, float cosLo, float roughness)
{
	float r = roughness + 1.0;
	float k = (r * r) / 8.0; // Epic suggests using this roughness remapping for analytic lights.
	return gaSchlickG1(cosLi, k) * gaSchlickG1(cosLo, k);
}

// Shlick's approximation of the Fresnel factor.
vec3 fresnelSchlick(vec3 F0, float cosTheta)
{
	return F0 + (vec3(1.0) - F0) * pow(1.0 - cosTheta, 5.0);
}

void main()
{
	// Sample input textures to get shading model params.
	vec3 albedo = f_albedo.xyz;
	float metalness = 0.0;
	float roughness = 0.1;

	// Outgoing light direction (vector from world-space fragment position to the "eye").
	vec3 Lo = normalize(cameraData.Position - f_worldPosition);
    vec3 Li = normalize(lightData.Position - f_worldPosition);

	// Get current fragment's normal and transform to world space.
	vec3 N = normalize(f_normal);
	//N = normalize(vin.tangentBasis * N);
	
	// Angle between surface normal and outgoing light direction.
	float cosLo = max(0.0, dot(N, Lo));
		
	// Specular reflection vector.
	vec3 Lr = 2.0 * cosLo * N - Lo;

	// Fresnel reflectance at normal incidence (for metals use albedo color).
	vec3 F0 = mix(vec3(0.04), albedo, metalness);

	// Direct lighting calculation for analytical lights.
	vec3 directLighting = vec3(0);
    // Light
    float distance = length(lightData.Position - f_worldPosition);
    float attenuation = 1.0 / pow(distance, 2.0);
    vec3 Lradiance = lightData.Color * attenuation;

    // Half-vector between Li and Lo.
    vec3 Lh = normalize(Li + Lo);

    // Calculate angles between surface normal and various light vectors.
    float cosLi = max(0.0, dot(N, Li));
    float cosLh = max(0.0, dot(N, Lh));

    // Calculate Fresnel term for direct lighting. 
    vec3 F  = fresnelSchlick(F0, max(0.0, dot(Lh, Lo)));
    // Calculate normal distribution for specular BRDF.
    float D = ndfGGX(cosLh, roughness);
    // Calculate geometric attenuation for specular BRDF.
    float G = gaSchlickGGX(cosLi, cosLo, roughness);

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
    directLighting += (diffuseBRDF + specularBRDF) * Lradiance * cosLi;

    // Approximation of ambiant
    vec3 ambientLighting = vec3(0.03) * albedo;
    vec3 finalColor = directLighting + ambientLighting;

    // Tone Mapping
    finalColor = finalColor / (finalColor + vec3(1.0));
    finalColor = pow(finalColor, vec3(1.0/2.2));
    
	// Final fragment color.
	o_color = vec4(finalColor, 1.0);
    o_entityID = f_entityID;
}