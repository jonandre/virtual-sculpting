#version 410 core
//#define Lightning

in vec3 normal;
in vec3 position;
in vec4 in_color;

#ifdef Lightning
	vec3 light = vec3( 100, 100, -100 );
	//vec3 lightHalfDir = light/2.0;
#endif

layout(location = 0) out vec4 out_Color;
void main(void)
{
    out_Color = in_color;
//return;
#ifdef Lightning
	{
	//out_Color.rgb = position/length(position);
		vec3 light_dir = normalize(light - position);		
		out_Color.rgb = out_Color.rgb*max(dot(normal, light_dir), 0.3);
	}
#endif		
}