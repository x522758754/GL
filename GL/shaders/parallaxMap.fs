#version 330 core

in VS_OUT
{
	vec2 texcoord;
	vec3 fragPos;
	vec3 lightDir;
	vec3 viewDir;
}fs_in;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_height1;
uniform float height_scale;

out vec4 fragColor;

vec2 ParallaxMapping(vec2 texcoord, vec3 viewDir)
{
	//
	const float numLayers = 10.0;
	float layerDepth = 1.0 / numLayers;
	//1根据坐标，2计算当前坐标应偏移的深度
	vec2  currentTexCoords = texcoord;
	float currentDepthMapValue = texture(texture_height1, currentTexCoords).r;
	//缩放UV偏移的总大小，对应贴图上的uv方向（UV偏移：视线在贴图上的投影，ViewDir.xy代表视线在贴图上的投影方向）
	vec2 offsetP = viewDir.xy * height_scale;
	//每次检测uv应偏移的增量
	vec2 deltaTexcoord = offsetP / numLayers;
	//从最小开始开始检测，直到当有层的对应的深度大于原有的深度
	float currentLayerDepth = 0.0;
	while(currentLayerDepth < currentDepthMapValue)
	{
		// shift texture coordinates along direction of P
		currentTexCoords -= deltaTexcoord;
		// get depthmap value at current texture coordinates
		currentDepthMapValue = texture(texture_height1, currentTexCoords).r;  
		// get depth of next layer
		currentLayerDepth += layerDepth;
	}
	//ViewDir.xy代表视线在贴图上的投影方向
	//vec2 offsetP = viewDir.xy / viewDir.z * ( height * height_scale);

	return texcoord - offsetP;
}

void main()
{
	vec2 texcoord = ParallaxMapping(fs_in.texcoord, normalize(fs_in.viewDir));
	// texcoord = vec2(clamp(texcoord.x, 0.0, 1.0), clamp(texcoord.y, 0.0, 1.0));
	// if(texcoord.x > 1.0 || texcoord.y > 1.0 || texcoord.x < 0.0 || texcoord.y < 0.0)
	// 	discard;
	vec3 color = texture(texture_diffuse1, texcoord).rgb;
	vec3 normal = normalize(texture(texture_normal1, texcoord).rgb * 2.0 - 1.0); //法线解压

	float ambient = 0.15;
	vec3 halfDir = normalize(fs_in.viewDir + fs_in.lightDir);
	float diffuse = max(0.0, dot(fs_in.lightDir, normal));
	float specular = pow(max(0.0, dot(halfDir, normal)), 8.0);
	color = (ambient + diffuse + specular) * color;
	fragColor = vec4(color, 1.0);
}