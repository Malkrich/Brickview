#type vertex
#version 450 core

layout (location = 0) in vec3 a_pos;

layout (std140, binding = 0) uniform CameraData
{
    mat4 ViewProjectionMatrix;
    mat4 View;
    mat4 Projection;
    vec3 Position;
} u_cameraData;

out vec3 f_localPosition;

void main()
{
    f_localPosition = a_pos;

    // remove translation from the view matrix
    mat4 rotView = mat4(mat3(u_cameraData.View));
    vec4 clipPos = u_cameraData.Projection * rotView * vec4(f_localPosition, 1.0);
    gl_Position = clipPos.xyww;
}

#type fragment
#version 450 core

layout (location = 0) out vec4 o_color;

layout (binding = 0) uniform samplerCube u_environmentCubemap;

in vec3 f_localPosition;

void main()
{
    vec3 normal = normalize(-f_localPosition);
    
    vec3 environmentColor = texture(u_environmentCubemap, normal).rgb;

    // Tone mapping
    environmentColor = environmentColor / (environmentColor + vec3(1.0));
    // Gamma correction
    environmentColor = pow(environmentColor, vec3(1.0/2.2));

    o_color = vec4(environmentColor, 1.0);
}