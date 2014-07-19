#version 330
precision highp float;
uniform sampler2D texture0;
in vec2 TexCoord;
out vec4 vFragColor;
uniform bool selected;
uniform bool active;
uniform bool temp;
uniform float flo;

void main(void) {
	vec4 col = selected?vec4(1,1,1,1):vec4(1);
	col = !selected?vec4(1.0, 1.0, 1.0, 0.2):vec4(2);

	if (active && selected && !temp)
		col = vec4(1.0, 1.0, 0.0,1);

	if (active && !selected && !temp)
		col = vec4(0.6, 0.6, 0.0,1);

	if (temp)
		col = vec4(1.0, 1.0, flo, 1);
		

	vFragColor = texture(texture0, TexCoord.st)*col;
}
