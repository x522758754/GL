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
	float quadratic;
};
uniform Light light;

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
	vec3 lightDir = light.position - fragPos;
	float diffuse = max(0.0, dot(normal, normalize(lightDir)));
	vec3 halfDir = normalize(lightDir + viewDir);
	float specular = pow(max(0.0, dot(normal, halfDir)), 8.0);
	float distance = length(lightDir);
	float attenuation = 1 / (1.0 + light.linear * distance + light.quadratic * distance * distance);
	vec3 lightColor = light.color * (specular + diffuse) * attenuation;
	vec3 ambient = 0.3 * albedo * blur;
	lightColor += ambient;

	FragColor = vec4(lightColor, 1.0);
}