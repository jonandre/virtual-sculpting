#version 410 core

in vec3 in_Position;
in vec4 in_Color;
in vec2 in_UV;
in vec3 in_Normal;

out vec3 position_vert;

void main(void)
{
    gl_Position = vec4(in_Position, 1.0);
	position_vert = in_Position;
}