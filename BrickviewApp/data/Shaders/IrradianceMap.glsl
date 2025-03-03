#type vertex
#version 450 core

layout (location = 0) in vec3 a_position;

out vec3 f_localPosition;

layout (std140, binding = 0) uniform CubemapData
{
    mat4 ViewProjectionMatrix;
} u_cubemapData;

void main()
{
    f_localPosition = a_position;
    gl_Position = u_cubemapData.ViewProjectionMatrix * vec4(f_localPosition, 1.0);
}

#type fragment
#version 450 core

#define PI 3.1415926538

layout (location = 0) out vec3 o_color;

in vec3 f_localPosition;

layout (binding = 0) uniform samplerCube u_environmentMap;

void main()
{
    vec3 normal = normalize(f_localPosition);
    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right = normalize(cross(up, normal));
    up = normalize(cross(normal, right));

    vec3 irradiance = vec3(0.0);
    float sampleDelta = 0.025;
    uint sampleCount = 0; 
    for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
    {
        for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
        {
            // spherical to cartesian (in tangent space)
            vec3 tangentSpaceSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
            // tangent space to world
            vec3 sampleVector = tangentSpaceSample.x * right + tangentSpaceSample.y * up + tangentSpaceSample.z * normal;

            irradiance += texture(u_environmentMap, sampleVector).rgb * cos(theta) * sin(theta);
            sampleCount++;
        }
    }

    irradiance = PI * irradiance * (1.0 / float(sampleCount));
    o_color = irradiance;
}