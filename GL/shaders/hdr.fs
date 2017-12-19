#version 330 core
in vec2 texcoord;
out vec4 fragColor;

uniform sampler2D texture_diffuse1;
uniform float exposure;

void main()
{
	const float gamma = 2.2;
	vec3 color = texture(texture_diffuse1, texcoord).rgb;
	// float average = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
	// color = vec3(average, average, average);
	//vec3 mapped = color / (color + vec3(1.0));
	vec3 mapped = vec3(1.0) - exp(-color * exposure);
	mapped = pow(mapped, vec3(1.0 / gamma));
	fragColor = vec4(mapped, 1.0);
}