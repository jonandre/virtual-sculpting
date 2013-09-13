#version 410 core

uniform mat4 p;
uniform mat4 v;
uniform mat4 m;

layout(points) in;
layout(triangle_strip, max_vertices=32) out;

layout(location = 0) in vec4 pass_Color_vert[];
layout(location = 1) in vec3 normal_vert[];
layout(location = 2) in vec3 position_vert[];

layout(location = 0) out vec4 pass_Color;

#if 1
	vec3 light = vec3( 0, 600, 0 );
	vec3 lightHalfDir = light/2.0;
	struct SHC{
    vec3 L00, L1m1, L10, L11, L2m2, L2m1, L20, L21, L22;
};

SHC groove = SHC(
    vec3( 0.3783264,  0.4260425,  0.4504587),
    vec3( 0.2887813,  0.3586803,  0.4147053),
    vec3( 0.0379030,  0.0295216,  0.0098567),
    vec3(-0.1033028, -0.1031690, -0.0884924),
    vec3(-0.0621750, -0.0554432, -0.0396779),
    vec3( 0.0077820, -0.0148312, -0.0471301),
    vec3(-0.0935561, -0.1254260, -0.1525629),
    vec3(-0.0572703, -0.0502192, -0.0363410),
    vec3( 0.0203348, -0.0044201, -0.0452180)
);

SHC beach = SHC(
    vec3( 0.6841148,  0.6929004,  0.7069543),
    vec3( 0.3173355,  0.3694407,  0.4406839),
    vec3(-0.1747193, -0.1737154, -0.1657420),
    vec3(-0.4496467, -0.4155184, -0.3416573),
    vec3(-0.1690202, -0.1703022, -0.1525870),
    vec3(-0.0837808, -0.0940454, -0.1027518),
    vec3(-0.0319670, -0.0214051, -0.0147691),
    vec3( 0.1641816,  0.1377558,  0.1010403),
    vec3( 0.3697189,  0.3097930,  0.2029923)
);

SHC tomb = SHC(
    vec3( 1.0351604,  0.7603549,  0.7074635),
    vec3( 0.4442150,  0.3430402,  0.3403777),
    vec3(-0.2247797, -0.1828517, -0.1705181),
    vec3( 0.7110400,  0.5423169,  0.5587956),
    vec3( 0.6430452,  0.4971454,  0.5156357),
    vec3(-0.1150112, -0.0936603, -0.0839287),
    vec3(-0.3742487, -0.2755962, -0.2875017),
    vec3(-0.1694954, -0.1343096, -0.1335315),
    vec3( 0.5515260,  0.4222179,  0.4162488)
);

vec3 sh_light(vec3 normal, SHC l){
    float x = normal.x;
    float y = normal.y; 
    float z = normal.z; 

    const float C1 = 0.429043;
    const float C2 = 0.511664;
    const float C3 = 0.743125;
    const float C4 = 0.886227;
    const float C5 = 0.247708;
    
    return (
        C1 * l.L22 * (x * x - y * y) +
        C3 * l.L20 * z * z +
        C4 * l.L00 -
        C5 * l.L20 +
        2.0 * C1 * l.L2m2 * x * y +
        2.0 * C1 * l.L21  * x * z +
        2.0 * C1 * l.L2m1 * y * z +
        2.0 * C2 * l.L11  * x +
        2.0 * C2 * l.L1m1 * y +
        2.0 * C2 * l.L10  * z
    );
}
#endif


const vec4 cubeVerts[8] = vec4[8](
    vec4(-0.5 , -0.5, -0.5,0),  //LB   0
     vec4(-0.5, 0.5, -0.5,0), //L T   1
    vec4(0.5, -0.5, -0.5,0), //R B    2
    vec4( 0.5, 0.5, -0.5,0),  //R T   3
                        //back face
    vec4(-0.5, -0.5, 0.5,0), // LB  4
     vec4(-0.5, 0.5, 0.5,0), // LT  5
    vec4(0.5, -0.5, 0.5,0),  // RB  6
     vec4(0.5, 0.5, 0.5,0)  // RT  7
    );

const int  cubeIndices[24]  = int [24]
    (
	  7,5,6,4,  //front
      0,1,2,3, //back
	  1,0,5,4, //left
      7,6,3,2, //right      
      4,0,6,2, //btm       
      3,1,7,5
    );

void EmitSide( vec4 pos, vec4 color, int start, vec4 transVerts[8], vec3 norm, bool do_light)
{
	mat4 pv = p*v;
	if ( do_light )
	{
		//vec3 light_dir = normalize(light - pos);
		for (int i=start;i<start+4; i++)
		{
			gl_Position = pv*(transVerts[cubeIndices[i]]);
			pass_Color.rgb = color.rgb*sh_light(norm, groove);
			//pass_Color.rgb = color.rgb*max(dot(norm, light_dir), 0.3);
			pass_Color.a = 1.0;			
			EmitVertex();
		}		
	}
	else
	{
		for (int i=start;i<start+4; i++)
		{
			gl_Position = pv*(transVerts[cubeIndices[i]]);
			pass_Color = color;
			EmitVertex();
		}
	}

}

vec3 GetNormal( uint code )
{
	vec3 normal_ = vec3(0.0,0.0,0.0);
	//is front
	//normal_[2] = ( code%2 == 0 ) ? 1.0 : 0;
	if ( code%2 == 0 )
	{
		normal_[2] += 1.0;
	}
	code = code>>1;
	//back
	if ( code%2 == 0 )
	{
		normal_[2] -= 1.0;
	}
	code = code>>1;
	//left
	if ( code%2 == 0 )
	{
		normal_[0] -= 1.0;
	}
	code = code>>1;
	//right
	if ( code%2 == 0 )
	{
		normal_[0] += 1.0;
	}
	code = code>>1;
	//btm
	if ( code%2 == 0 )
	{
		normal_[1] -= 1.0;
	}
	code = code>>1;
	//top
	if ( code%2 == 0 )
	{
		normal_[1] += 1.0;
	}
	return -normal_;
}

	
void EmitCube( vec4 pos, vec4 color, uint in_code )
{
	uint code = in_code;
	vec4 transVerts[8];
	for (int i=0;i<8; i++) 
	{
		transVerts[i]= (m)*(pos + cubeVerts[i]);
	}
	vec3 normal_ = GetNormal(code);
	normal_ = normalize((m*vec4(normal_, 1.0))).xyz;
	for (int i=0;i<6;i++)
    {
		if ( code%2 == 1 )
		{
			EmitSide( pos, color, i*4, transVerts, normal_, true);
			EndPrimitive();
		}
		code = code>>1;
    }
	//return;	
	//normal_ = vec3( 0.0, -1.0, 0.0 );
	color = vec4( 0.3,0.3,0.3,1.0 );
	/*if ((in_code>>5)%2 == 1)
	{
	//3,1,7,5
		const float flor = -193.0;
		transVerts[1]= (pos + cubeVerts[1]);
		transVerts[1].y = flor;
		//transVerts[1]= (vm)*transVerts[1];
		transVerts[3]= (pos + cubeVerts[3]);
		transVerts[3].y = flor;
		//transVerts[3]= (vm)*transVerts[3];
		transVerts[7]= (pos + cubeVerts[7]);
		transVerts[7].y = flor;
		//transVerts[7]= (vm)*transVerts[7];
		transVerts[5]= (pos + cubeVerts[5]);
		transVerts[5].y = flor;
		//transVerts[5]= (vm)*transVerts[5];
		
		EmitSide( pos, color,20, transVerts, normal_, false);
		EndPrimitive();	
	}
	
	return;*/
	code = in_code;
	for (int i=0;i<8; i++) 
	{
		transVerts[i]= (pos + cubeVerts[i]);
		transVerts[i]= (m)*transVerts[i];
		transVerts[i].y = -193.0;
	}
	for (int i=0;i<6;i++)
    {
		if ( code%2 == 1 )
		{
			EmitSide( pos, color, i*4, transVerts, normal_, false);
			EndPrimitive();
		}
		code = code>>1;
    }
    
}

void main()
{	
  EmitCube(gl_in[0].gl_Position, pass_Color_vert[0]/255.0, uint(pass_Color_vert[0][3]));
}  