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

	float bias = 0.05;
	float shadow = 0;
	float offset = 0.1;
	float samples = 4;
	for(float x = -offset; x < offset; x += offset / (samples * 0.5))
	{
		for(float y = -offset; y < offset; y += offset / (samples * 0.5))
		{
			for(float z = -offset; z < offset; z += offset / (samples * 0.5))
			{
				float closestDpeth = texture(depthCubemap, fragToLight + vec3(x,y,z)).r;
				float closestDist = closestDpeth * far_plane;
				float currentDist = length(lightPos - fs_in.position);
				if(currentDist - bias > closestDist)
					shadow += 1.0;
			}
		}
	}
	shadow /= float(pow(samples,3));


	//shadow = 0.0;
	//out
	vec3 texColor = texture(tex2D, fs_in.texcoord).rgb;
	vec3 lightColor = vec3(0.3);

	fragColor = vec4(texColor * lightColor, 1.0) * ((1.0- shadow) * (diffuse + spec) + ambient);
	//fragColor = vec4(vec3(pow(closestDpeth, 2.0)), 1.0);
}