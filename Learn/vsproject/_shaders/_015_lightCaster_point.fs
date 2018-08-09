#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

struct Material
{
	vec3 ambient;
	sampler2D diffuse; //漫反射光照下物体的颜色
	sampler2D specular; //镜面光照对物体的颜色影响
	float shininess; //影响镜面高光的散射/半径。
};
uniform Material material;

struct Light {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant; //1.0
	float linear;	//0.09
	float quadratic;//0.032
};

uniform Light light;

uniform vec3 viewPos;

void main()
{
	float distance    = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	// 环境光
	vec3 ambient = light.ambient * material.ambient * attenuation;

	// 漫反射 
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * (diff * vec3(texture(material.diffuse, TexCoords))) * attenuation;

	// 镜面光
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);  
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * (spec * vec3(texture(material.specular, TexCoords))) * attenuation;

	vec3 result = diffuse + specular;
	FragColor = vec4(result, 1.0);
}