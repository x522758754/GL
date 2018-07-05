#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in VS_OUT{
	vec3 position;
	vec3 normal;
	vec2 texCoords;
}fs_in;

struct Light
{
	vec3 position;
	vec3 color;
};

#define NR_POINT_LIGHTS 4

uniform Light lights[NR_POINT_LIGHTS];

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform vec3 eyePos;

vec3 CalcLight(Light light)
{
	float max_distance = 1.5;
	float distance = length(light.position - fs_in.position);
	float attenuation = 1.0 / distance * distance;

	vec3 normal = normalize(fs_in.normal);
	vec3 texDiff = texture(texture_diffuse1, fs_in.texCoords).rgb;
	vec3 texSpec = texture(texture_specular1, fs_in.texCoords).rgb;

	vec3 ambient = 0.0 * texDiff;

	vec3 lightDir = normalize(light.position - fs_in.position);
	float diff = max(0, dot(lightDir, normal));
	vec3 diffuse = diff * light.color * texDiff * attenuation;

	//Blinn-Phong着色的一个附加好处是，它比Phong着色性能更高，因为我们不必计算更加复杂的反射向量了。
	vec3 viewDir = normalize(eyePos - fs_in.position);
	vec3 halfwayDir = normalize(viewDir + lightDir);
	float spec = pow(max(0, dot(halfwayDir, normal)), 32);

	vec3 specular = spec * light.color * texSpec * attenuation;

	vec3 color = ambient + diffuse + specular;

	return color;
}


void main()
{
	vec3 color = vec3(0.0);
	for(int i=0; i < NR_POINT_LIGHTS; ++i)
	{
		color += CalcLight(lights[i]);
	}

	FragColor = vec4(color, 1.0);
	//泛光在HDR基础上能够运行得很好的原因。(使用浮点帧缓冲可以存储超过0.0到1.0范围的浮点值，所以非常适合HDR渲染)
	//因为HDR中，我们可以将颜色值指定超过1.0这个默认的范围，我们能够得到对一个图像中的亮度的更好的控制权。
	//没有HDR我们必须将阈限设置为小于1.0的数，虽然可行，但是亮部很容易变得很多，这就导致光晕效果过重。
	float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        BrightColor = vec4(FragColor.rgb, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}