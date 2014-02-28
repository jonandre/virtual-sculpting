#version 330
precision highp float;
uniform sampler2D texture0;
in vec2 TexCoord;
out vec4 vFragColor;
uniform bool selected;

void main(void) {
	vec4 col=selected?vec4(1,0,0,1):vec4(1);
	vFragColor = texture(texture0, TexCoord.st)*col;
	if (vFragColor.rgb == vec3(0.0, 0.0, 0.0)) {
		discard;
	}
}
