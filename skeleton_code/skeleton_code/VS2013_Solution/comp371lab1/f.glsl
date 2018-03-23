#version 330 core

out vec4 color;

in vec2 TexCoord;
in vec3 Normal; 
in vec3 FragPos;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform int fill;
uniform int shader_x_pressed;
uniform vec3 lightColor;
uniform vec3 lightPos; 
uniform vec3 viewPos;


void main()
{
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColor;
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos); 
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;
	float specularStrength = 0.5;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);  
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 256);
	vec3 specular = specularStrength * spec * lightColor;  
	//0 is ORANGE
	if(shader_x_pressed == 1){
		if (fill == 0) {
			color = texture(texture1, TexCoord) * vec4((specular + ambient + diffuse) * vec3(1.000f, 0.647f, 0.000f), 1.0f);
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
			color = texture(texture1, TexCoord) * vec4((specular + ambient + diffuse) * vec3(0.545f, 0.271f, 0.075f), 1.0f);
		}
		if (fill == 6) {
			color = texture(texture1, TexCoord) * vec4((specular + ambient + diffuse) * vec3(0.647f, 0.165f, 0.165f), 1.0f);
		}
		if (fill == 7){
			color = texture(texture2, TexCoord)* vec4((specular + ambient + diffuse),1.0f);
		}
	}else{
	if (fill == 0) {
			color = vec4((specular + ambient + diffuse) * vec3(1.000f, 0.647f, 0.000f), 1.0f);
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
			color = vec4((specular + ambient + diffuse) * vec3(0.545f, 0.271f, 0.075f), 1.0f);
		}
		if (fill == 6) {
			color = vec4((specular + ambient + diffuse) * vec3(0.647f, 0.165f, 0.165f), 1.0f);
		}
		if (fill == 7){
			color = texture(texture2, TexCoord);
		}
	}
	
}