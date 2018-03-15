#version 330 core

out vec4 color;

in vec2 TexCoord;

uniform sampler2D texture1;
uniform int fill;

void main()
{
	//0 is ORANGE
	if (fill == 0) {
		color = texture(texture1, TexCoord) * vec4(1.000f, 0.647f, 0.000f, 1.0f);
	}
	//1 is WHITE
	if (fill == 1) {
		color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	//2 is RED
	if (fill == 2) {
		color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	}
	//3 is GREEN
	if (fill == 3) {
		color = vec4(0.0f, 1.0f, 0.0f, 1.0f);
	}
	//4 is BLUE
	if (fill == 4) {
		color = vec4(0.0f, 0.0f, 1.0f, 1.0f);
	}
	if (fill == 5) {
		color = texture(texture1, TexCoord) * vec4(0.545f, 0.271f, 0.075f, 1.0f);
	}
	if (fill == 6) {
		color = texture(texture1, TexCoord) * vec4(0.647f, 0.165f, 0.165f, 1.0f);
	}
}