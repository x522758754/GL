#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D bloomBlur;

uniform bool bloom;
uniform float exposure;

void main()
{ 
	const float gamma = 2.2;
	vec3 hdrColor = texture(screenTexture, TexCoords).rgb;
	vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;

	if(bloom)
		hdrColor += bloomColor;

	// Reinhard色调映射
	//vec3 mapped = hdrColor / (hdrColor + vec3(1.0));
	//曝光色调映射
	vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
	// Gamma校正
	mapped = pow(mapped, vec3(1.0 / gamma));

	FragColor = vec4(mapped, 1.0);
}