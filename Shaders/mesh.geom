#version 410 core

uniform mat4 pvm;

layout(triangles) in;
layout(triangle_strip, max_vertices=6) out;

in vec3 position_vert[];

out vec3 normal;
out vec3 position;
out vec4 in_color;

void main()
{
	const float near_depth = 192.0;
	vec3 a = gl_in[0].gl_Position.xyz - gl_in[1].gl_Position.xyz;
	vec3 b = gl_in[0].gl_Position.xyz - gl_in[2].gl_Position.xyz;
	//vec3 ñ = gl_in[1].gl_Position.xyz - gl_in[2].gl_Position.xyz;
	//if((( gl_in[0].gl_Position.z == near_depth) || (gl_in[1].gl_Position.z  == near_depth)) || gl_in[2].gl_Position.z == near_depth )
	//if(( gl_in[0].gl_Position.z == gl_in[1].gl_Position.z ) && (gl_in[2].gl_Position.z == gl_in[1].gl_Position.z) )
	if((( gl_in[0].gl_Position.z == near_depth) && (gl_in[1].gl_Position.z  == near_depth)) && gl_in[2].gl_Position.z == near_depth )
		return;
	
	vec3 nrm = normalize(cross( a, b ));
	
	for (int i=0;i<3; i++) 
	{
		gl_Position = pvm*(gl_in[i].gl_Position);
		normal = nrm;
		in_color = vec4(0.0,1.0,0.0,(-gl_in[i].gl_Position.z/near_depth+ 1.0)/2.0);
		position = position_vert[i];
		EmitVertex();
	}
	EndPrimitive();
	for (int i=0;i<3; i++) 
	{
		gl_Position = pvm*vec4(gl_in[i].gl_Position.x, -near_depth, gl_in[i].gl_Position.z, 1.0);
		normal = nrm;
		in_color = vec4(0.0,1.0,0.0,1.0);
		position = position_vert[i];
		EmitVertex();
	}
	EndPrimitive();
}  