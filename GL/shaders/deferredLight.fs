//延迟渲染 光照处理
#version 330 core
out vec4 FragColor;

in vec2 texcoord;
uniform sampler2D texture_gPosition;
uniform sampler2D texture_gNormal;
uniform sampler2D texture_gAlbedoSpec;

const int lightCount = 4;
uniform vec3 lightPositions[lightCount];
uniform vec3 lightColors[lightCount];
uniform vec3 viewPos;

void main()
{
	vec3 fragPos = texture(texture_gPosition, texcoord).rgb;
	vec3 normal = texture(texture_gNormal, texcoord).rgb;
	vec4 albedoSpec = texture(texture_gAlbedoSpec, texcoord) * 0.2;
	normal = normalize(normal);
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 result = vec3(0.0);
	for(int i=0; i != lightCount; ++i)
	{
		vec3 lightDir = lightPositions[i] - fragPos;
		float distance = length(lightDir);
		float attenuation = 1; // / distance * distance;
		lightDir = normalize(lightDir);
		float diffuse = max(0, dot(normal, lightDir)) * attenuation;
		vec3 halfDir = normalize(viewDir + lightDir);
		float specular = pow(max(0, dot(halfDir, normal)), albedoSpec.a);
		result += (diffuse + specular) * lightColors[i] * albedoSpec.rgb;
	}
	FragColor = vec4(result, 1.0);
}