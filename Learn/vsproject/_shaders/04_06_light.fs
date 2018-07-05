#version 330 core
out vec4 FragColor;

in VS_OUT{
	vec3 position;
	vec3 normal;
	vec2 texCoords;
}fs_in;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 eyePos;



void main()
{
	const float shininess = 8.0;
	const int switchOn = 0;
	const bool gamma = false;

	float max_distance = 1.5;
	float distance = length(lightPos - fs_in.position);
	float attenuation = 1.0 / (gamma ? distance * distance : distance);

	vec3 normal = normalize(fs_in.normal);
	vec3 texDiff = texture(texture_diffuse1, fs_in.texCoords).rgb;
	vec3 texSpec = texture(texture_specular1, fs_in.texCoords).rgb;

	vec3 ambient = vec3(0.1, 0.1, 0.1) * texDiff;

	vec3 lightDir = normalize(lightPos - fs_in.position);
	float diff = max(0, dot(lightDir, normal));
	vec3 diffuse = diff * lightColor * texDiff * attenuation;

	//Blinn-Phong着色的一个附加好处是，它比Phong着色性能更高，因为我们不必计算更加复杂的反射向量了。
	vec3 viewDir = normalize(eyePos - fs_in.position);
	vec3 halfwayDir = normalize(viewDir + lightDir);
	float spec = pow(max(0, dot(halfwayDir, normal)), shininess * 4);

	//phong
	if(switchOn > 0)
	{
		vec3 reflect = reflect(-lightDir, normal);
		spec = pow(max(0, dot(reflect, viewDir)), shininess);
	}

	vec3 specular = spec * lightColor * texSpec * attenuation;

	vec3 color = diffuse + specular;
	if (gamma)
		color = pow(color, vec3(1.0/2.2));

	FragColor = vec4(color, 1.0);
}