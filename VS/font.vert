#version 330

uniform mat4 MVP;

in vec2 vVertex;
in vec2 vTexCoord;
out vec2 TexCoord;

void main(void) {
	TexCoord = vTexCoord;		
	gl_Position = MVP * vec4(vVertex, 0.0, 1.0);		
}
