layout(std140, binding = 0) uniform CameraData
{
  mat4 ViewProjectionMatrix;
  mat4 View;
  mat4 Projection;
  vec3 Position;
}
u_CameraData;
