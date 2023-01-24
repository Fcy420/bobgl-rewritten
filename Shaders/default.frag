#version 330 core

// Outputs colors in RGBA
out vec4 FragColor;
in vec2 texCoord;
in vec4 color;
uniform sampler2D tex0;
void main()
{
	vec4 final = vec4(texture(tex0, texCoord).rgb*color.rgb, color.a);
	if(final.a < 0.1) {
		discard;
	}
	FragColor = final;
}