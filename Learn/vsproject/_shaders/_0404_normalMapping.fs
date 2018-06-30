#version 330 core
out vec4 FragColor;

in VS_OUT{
	vec3 vertexPos;
	vec3 lightPos;
	vec3 viewPos;
	vec2 texCoords;
}fs_in;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_specular1;

vec3 calcDirLight(vec3 lightDir, vec3 normal, vec3 viewDir)
{

	vec3 diffuseColor = texture(texture_diffuse1, fs_in.texCoords).rgb;
	vec3 specularColor = texture(texture_specular1, fs_in.texCoords).rgb;
	//ambient
	vec3 ambient = 0.1 * diffuseColor;

	//diffuse
	float diff = max(0.0, dot(lightDir, normal));
	vec3 diffuse = diff * diffuseColor;
	
	//specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(0.0, dot(reflectDir, viewDir)), 32);
	//vec3 halfwayDir = normalize(lightDir + viewDir);
	//float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
	vec3 specular = spec * vec3(.5) * specularColor;

	vec3 result = ambient + diffuse + specular;
	return result;
}


void main()
{
	vec3 lightDir = normalize(fs_in.lightPos - fs_in.vertexPos);
	vec3 viewDir = normalize(fs_in.viewPos - fs_in.vertexPos);

	vec3 normal = texture(texture_normal1, fs_in.texCoords).rgb;
	normal = normalize(normal * 2 - 1.0);

	vec3 color = calcDirLight(lightDir, normal, viewDir);

	FragColor = vec4(color, 1.0);
}