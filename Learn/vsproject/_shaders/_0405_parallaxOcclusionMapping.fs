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

	//我们先定义层的数量，计算每一层的高度，最后计算纹理坐标偏移，每一层我们必须沿着P¯的方向进行移动。
	//然后我们遍历所有层，从上开始，直到找到层高度小于其对应的高度图上的高度
   const float minLayers = 4;
    const float maxLayers = 10;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));  
    float layerDepth = 1.0 / numLayers;
    float currentLayerDepth = 0.0;
    vec2 P = viewDir.xy / viewDir.z * height_scale; 
    vec2 deltaTexCoords = P / numLayers;

	//初始纹理坐标和其对应高度图上的高度
	vec2 currentTexCoords = texCoords;
	float currentDepthMapValue = texture(texture_height1, currentTexCoords).r;

	//从0层开始遍历,判断当前层的高度值是否小于其对应的高度图上的高度
	while(currentLayerDepth < currentDepthMapValue)
	{
		//获取下一层的高度值
		currentLayerDepth += layerDepth;
		// shift texture coordinates along direction of P
		//沿着视线的方向偏移检索的纹理坐标
		currentTexCoords -= deltaTexCoords;
		//获取当前偏移后的高度图上的高度值
		currentDepthMapValue = texture(texture_height1, currentTexCoords).r;
	}

	//获取之前一层的高度对应的纹理坐标
	vec2 prevTexCoords = currentTexCoords + deltaTexCoords;
	//获取当前层对应的高度图上的贴图值与当前层的的高度值的差值
	float afterDepth  = currentDepthMapValue - currentLayerDepth;
	
	//获取之前层对应的高度图上的贴图值与之前层的的高度值的差值
	float beforeDepth = texture(texture_height1, prevTexCoords).r - currentLayerDepth + layerDepth;

	//纹理坐标插值在prevTexCoords和currentTexCoords之间插值
	float weight = afterDepth / (afterDepth - beforeDepth);
	vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

	return finalTexCoords;
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