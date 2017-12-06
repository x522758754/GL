#version 330 core

in VS_OUT
{
	vec3 position;
	vec3 normal;
	vec2 texcoord;
} fs_in;

uniform sampler2D tex2D;
uniform samplerCube depthCubemap;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float far_plane;
out vec4 fragColor;

void main()
{
	//基础数据
	vec3 normal = normalize(fs_in.normal);
	vec3 lightDir = normalize(lightPos - fs_in.position);
	vec3 viewDir = normalize(viewPos - fs_in.position);
	vec3 halfDir = normalize(viewDir + lightDir);
	//光照系数
	float ambient = 0.25;
	float diffuse = max(0, dot(normal, halfDir));
	float spec = pow(diffuse, 32);
	//阴影,有阴影则只会计算环境光
	vec3 fragToLight = fs_in.position - lightPos;
	float closestDpeth = texture(depthCubemap, fragToLight).r;
	float closestDist = closestDpeth * far_plane;
	vec3 distLight2FragPos = lightPos - fs_in.position;
	float currentDist = length(distLight2FragPos);
	float bias = 0.05;
	float shadow = currentDist - bias > closestDist ? 1.0: 0.0;
	//shadow = 0.0;
	//out
	vec3 texColor = texture(tex2D, fs_in.texcoord).rgb;
	vec3 lightColor = vec3(0.3);

	fragColor = vec4(texColor * lightColor, 1.0) * ((1.0- shadow) * (diffuse + spec) + ambient);
	fragColor = vec4(vec3(closestDpeth), 1.0);
}