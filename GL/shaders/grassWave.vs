#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexcoord;
layout(location = 5) in vec3 aColor;

out VS_OUT
{
	vec3 position;
	vec3 normal;
	vec2 texcoord;
	vec3 color;
}vs_out;

// uniform float windWave; 	//风偏移最大值
// uniform float windSpeed; 	//
uniform float time; 		//
// uniform vec3 rootPos;		//根位置,本地坐标
// uniform float xOffset;		//x偏移值
// uniform float zOffset;		//z偏移值
uniform vec3 dstPos;		//目标位置(玩家当前的世界坐标),世界坐标
// uniform float bendMinDist; 	//使弯曲的最小距离
// uniform float bendScale;	//弯曲的程度参数

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

const float windWave = 1;
const float windSpeed = 1;
const vec3 rootPos = vec3(1000, 0, 1000);
const float xOffset = 1;
const float zOffset = 1;
const float bendMinDist = 1;
const float bendScale = 1;

void main()
{
	//模拟风吹草动
	float windBend = sin(length(aPos - rootPos) * windWave + time * 10 * windSpeed);

	//http://blog.csdn.net/qq18052887/article/details/50995928
	//用顶点颜色(aColor)或者UV值（aTexcoord.y ）来做权重作为草的根部和顶部摆动权重
	vec3 newPos = aPos + vec3(xOffset, 0, zOffset) * windBend * 0.1 * aTexcoord.y * aColor;

	//人走过时的草动
	vec3 worldPos = mat3(model) * newPos;
	vec3 bendDir = vec3(worldPos.x, 0, worldPos.z) - vec3(dstPos.x, 0, dstPos.z);
	vec3 bendDirNorm  = normalize(bendDir);
	float bend = (bendMinDist - min(bendMinDist, length(bendDir))) / bendMinDist;
	newPos.xz = newPos.xz + bendDirNorm.xz * bend * aTexcoord.y * aTexcoord.y * bendScale;

	gl_Position = projection * view * model * vec4(newPos, 1.0);
	vs_out.texcoord = aTexcoord;
	vs_out.color = aColor;
}