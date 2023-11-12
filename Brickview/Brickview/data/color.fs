#version 330 core

in vec3 vertexColor;

out vec4 color;

void main()
{
	color = vec4(vertexColor.x, vertexColor.y, vertexColor.z, 1.0f);
};
