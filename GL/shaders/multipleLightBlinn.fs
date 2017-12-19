#version 330 core

in VS_OUT
{
	vec3 position;
	vec3 normal;
	vec2 texcoord;
} fs_in;

uniform sampler2D texture_diffuse1;
uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];
uniform vec3 viewPos;

out vec4 fragColor;

void main()
{
	//基础数据
	vec3 normal = normalize(fs_in.normal);
	//光照系数
	float ambient = 0.25;
	float diffuse = 0.0; 
	float specular = 0.0;
	vec3 lightColor = vec3(0.0);
	for(int i=0; i < 4; ++i)
	{
		vec3 lightDir = lightPositions[i] - fs_in.position;
		float distance = length(lightDir);
		float attenuation = 1.0 / (distance * distance);
		lightDir = normalize(lightDir);
		diffuse = max(0, dot(normal, lightDir));
		lightColor += diffuse * lightColors[i] * attenuation;
		//vec3 viewDir = normalize(viewPos - fs_in.position);
		//vec3 halfDir = normalize(viewDir + lightDir);
		//specular = pow(max(0, dot(normal, halfDir)));
	}
	vec3 texColor = texture(texture_diffuse1, fs_in.texcoord).rgb;
	vec3 ambientColor = ambient * texColor;
	lightColor *= texColor;

	fragColor = vec4(ambientColor + lightColor, 1.0);
}