//1.以cube的locaPos插值作为方向向量
//2.该方向向量(3d)通过三角函数转换成uv(2d)索引,检索球形投影 --原理不明
//3.直接将检索的值存储为cube的面上
#version 330 core

out vec4 FragColor;
in vec3 localPos;

uniform sampler2D equirectangularMap;
const vec2 invAtan = vec2(0.1591, 0.3183);

//从3维到2维，获得对球形贴图取样uv
vec2 SampleShpericalMap(vec3 v)
{
	vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
	uv *= invAtan;
	uv += 0.5;
	return uv;
}


void main()
{
	vec3 viewDir = normalize(localPos); //make sure to normalize
	vec2 uv = SampleShpericalMap(viewDir);
	vec3 color = texture(equirectangularMap, uv).rgb;

	FragColor = vec4(color, 1.0);
}