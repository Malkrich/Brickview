#type vertex
#version 450 core

layout(location = 0) in vec2 a_Position;

out vec2 f_TextCoord;

void main()
{
  f_TextCoord = a_Position * 0.5 + 0.5;
  gl_Position = vec4(a_Position, 0.0, 1.0);
}

#type fragment
#version 450 core

#include "includes/PBR/Utils.glsl"

in vec2 f_TextCoord;

out vec2 o_Color;

//----------------------------------------------------------------------------
void main()
{
  o_Color = IntegrateBRDF(max(f_TextCoord.x, 0.0001), f_TextCoord.y);
}
