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

	//我们先定义层的数量，计算每一层的深度，最后计算纹理坐标偏移，每一层我们必须沿着P¯的方向进行移动。
	//然后我们遍历所有层，从上开始，直到找到小于这一层的深度值的深度贴图值：
   const float minLayers = 4;
    const float maxLayers = 10;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));  
    float layerDepth = 1.0 / numLayers;
    float currentLayerDepth = 0.0;
    vec2 P = viewDir.xy / viewDir.z * height_scale; 
    vec2 deltaTexCoords = P / numLayers;

	// get initial values
	vec2 currentTexCoords = texCoords;
	float currentDepthMapValue = texture(texture_height1, currentTexCoords).r;

	while(currentLayerDepth < currentDepthMapValue)
	{
		// shift texture coordinates along direction of P
		currentTexCoords -= deltaTexCoords;
		// get depthmap value at current texture coordinates
		currentDepthMapValue = texture(texture_height1, currentTexCoords).r;
		// get depth of next layer
		currentLayerDepth += layerDepth;
	}

	return currentTexCoords;
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