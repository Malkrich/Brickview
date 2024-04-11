#version 330 core

in vec3 f_normal;

out vec4 color;

void main()
{
    color = vec4(f_normal, 1.0);
    //color = vec4(0.8, 0.2, 0.2, 1.0);
};
