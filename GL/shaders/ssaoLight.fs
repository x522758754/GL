#version 330 core
out vec4 FragColor;
in vec2 texcoord;

uniform sampler2D texture_gPositionDepth;
uniform sampler2D texture_gNormal;
uniform sampler2D texture_gAlbedo;
uniform sampler2D texture_ssaoBlur;

struct Light{
	vec3 position;
	vec3 color;

	float linear;
}

void main()
{
	vec4 fragPosDepth = texture(texture_gPositionDepth, texcoord);
	vec3 normal = texture(texture_gNormal, texcoord).rgb;
	vec3 albedo = texture(texture_gAlbedo, texcoord).rgb;
	float blur = texture(texture_ssaoBlur, texcoord).r;
	vec3 fragPos = fragPosDepth.rgb;
	// calc blinn-phong in view space
	vec3 viewPos = vec3(0.0); 
	vec3 viewDir = viewPos - fragPos;
	vec3 lightDir = light.position 
	float diffuse = max(0, dot(normal, lightDir));
}