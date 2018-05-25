#version 330 core
out vec4 FragColor;

in vec3 normal;
in vec3 fragPos;

uniform vec3 lightPos;
uniform vec3 eyePos;
uniform vec3 objectColor;
uniform vec3 lightColor;

void main()
{
	//ambient
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColor;
	//diffuse
	vec3 norm = normalize(normal); 
	vec3 lightDir = normalize(lightPos - fragPos);
	vec3 diffuse = max(0.0, dot(lightDir, norm)) * lightColor;
	//specular
	float specularStrength = 0.5; //镜面反射强度
	float shininess = 128.0; //一个物体的发光值越高，反射光的能力越强，散射得越少，高光点越小。
	vec3 viewDir = normalize(eyePos - fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	vec3 specular = pow(max(0.0, dot(viewDir, reflectDir)), shininess) * lightColor * specularStrength;

	vec3 result = (ambient + diffuse + specular) * objectColor;;
	//vec3 result = diffuse * objectColor;
	FragColor = vec4(result, 1.0);
}