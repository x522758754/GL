#version 330 core
out vec4 FragColor;

in VS_OUT{
	vec3 vertexPos;
	vec3 lightPos;
	vec3 viewPos;
	vec2 texCoords;
}fs_in;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_height1;
//uniform float height_scale; //用来人为调控

vec3 calcDirLight(vec3 lightDir, vec3 normal, vec3 viewDir, vec2 texCoords)
{
	vec3 diffuseColor = texture(texture_diffuse1, texCoords).rgb;
	vec3 specularColor = texture(texture_specular1, texCoords).rgb;
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


vec2 ParallaxMapping(vec3 viewDir, vec2 texCoords)
{
	float height_scale = 0.1;

	// 高度图中描述的高度数据
	// 根据视角的方向，用来偏移当前纹理坐标
	float height = texture(texture_height1, texCoords).r;
	//因为viewDir向量是经过了标准化的，viewDir.z会在0.0到1.0之间的某处。
	//当viewDir大致平行于表面时，它的z元素接近于0.0，除法会返回比viewDir垂直于表面的时候更大观察偏移的向量。
	//这个技术：Parallax Mapping with Offset Limiting
	//也可以不使用,就是普通视差贴图
	vec2 viewDir_xy_enlarge = viewDir.xy / viewDir.z; //Parallax Mapping with Offset Limiting

	//使用三维的视线向量投影到二维的 uv 平面，即取xy分量
	vec2 p = viewDir.xy * height * height_scale;

	return texCoords - p;
}


void main()
{
	vec3 lightDir = normalize(fs_in.lightPos - fs_in.vertexPos);
	vec3 viewDir = normalize(fs_in.viewPos - fs_in.vertexPos);

	vec2 texCoords = ParallaxMapping(viewDir, fs_in.texCoords);
	if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
		discard;

	vec3 normal = texture(texture_normal1, texCoords).rgb;
	normal = normalize(normal * 2 - 1.0);

	vec3 color = calcDirLight(lightDir, normal, viewDir, texCoords);

	FragColor = vec4(color, 1.0);
}