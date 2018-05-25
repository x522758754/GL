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
	vec3 position; //聚光灯位置，用来算LightDir
	vec3 direction;//聚光所指的方向
	float cutOff;  //聚光灯的内圆锥角余弦值；光在内圆锥亮度不变
	float outerCutOff; //聚光灯的外圆锥角余弦值；光从内圆锥逐渐变暗，直到外圆锥的边界

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform Light light;

uniform vec3 viewPos;

void main()
{
	vec3 lightDir = normalize(light.position - FragPos);
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon   = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0); 

	// 执行光照计算
	// 环境光
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

	// 漫反射 
	vec3 norm = normalize(Normal);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * (diff * vec3(texture(material.diffuse, TexCoords))) * intensity;

	// 镜面光
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);  
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * (spec * vec3(texture(material.specular, TexCoords))) * intensity;

	vec3 result = ambient + diffuse + specular;
	FragColor = vec4(result, 1.0);
}