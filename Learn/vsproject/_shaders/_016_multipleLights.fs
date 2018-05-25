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

//平行光
struct DirLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};  
uniform DirLight dirLight;

//点光源
struct PointLight {
	vec3 position;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
#define NR_POINT_LIGHTS 2
uniform PointLight pointLights[NR_POINT_LIGHTS];


//聚光灯
struct SpotLight {
	vec3 position; //聚光灯位置，用来算LightDir
	vec3 direction;//聚光所指的方向
	float cutOff;  //聚光灯的内圆锥角余弦值；光在内圆锥亮度不变
	float outerCutOff; //聚光灯的外圆锥角余弦值；光从内圆锥逐渐变暗，直到外圆锥的边界

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform SpotLight spotLight;

uniform vec3 viewPos;

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	//ambient
	vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

	//diffuse
	vec3 lightDir = normalize(-light.direction);
	float diff = max(0.0, dot(lightDir, normal));
	vec3 diffuse = diff * light.diffuse * texture(material.diffuse, TexCoords).rgb;
	
	//specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(0.0, dot(reflectDir, viewDir)), material.shininess);
	vec3 specular = spec * light.specular * texture(material.specular, TexCoords).rgb;

	vec3 result = ambient + diffuse + specular;
	return result;
}


vec3 calcPointLight(PointLight light, vec3 normal, vec3 viewDir)
{
	float distance    = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	//ambient
	vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

	//diffuse
	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(0.0, dot(lightDir, normal));
	vec3 diffuse = diff * light.diffuse * texture(material.diffuse, TexCoords).rgb * attenuation;

	//specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(0.0, dot(reflectDir, viewDir)), material.shininess);
	vec3 specular = spec * light.specular * texture(material.specular, TexCoords).rgb * attenuation;

	vec3 result = ambient + diffuse + specular;
	return result;
}

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - FragPos);
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon   = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0); 

	//ambient
	vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

	//diffuse
	float diff = max(0.0, dot(lightDir, normal));
	vec3 diffuse = diff * light.diffuse * texture(material.diffuse, TexCoords).rgb * intensity;

	//specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(0.0, dot(reflectDir, viewDir)), material.shininess);
	vec3 specular = light.specular * (spec * vec3(texture(material.specular, TexCoords))) * intensity;

	vec3 result = ambient + diffuse + specular;
	return result;
}

void main()
{
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);
	//direction
	vec3 result = calcDirLight(dirLight, norm, viewDir);
	//point
	for(int i=0; i < NR_POINT_LIGHTS; ++i)
	{
		result += calcPointLight(pointLights[i], norm, viewDir);
	}
	//spot
	result += calcSpotLight(spotLight, norm, viewDir);
	FragColor = vec4(result, 1.0);
}