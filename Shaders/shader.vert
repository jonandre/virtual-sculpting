#version 410 core

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec4 in_Color;
layout(location = 2) in vec2 in_UV;
layout(location = 3) in vec3 in_Normal;

layout(location = 0) out vec4 pass_Color_vert;
layout(location = 1) out vec3 normal_vert;
layout(location = 2) out vec3 position_vert;

void main(void)
{
    gl_Position = vec4(in_Position, 1.0);
	position_vert = in_Position;
	normal_vert = in_Normal;
    pass_Color_vert = (in_Color);///vec4(255.0,255.0,255.0,255.0);
	//UV_vert = in_UV;
}