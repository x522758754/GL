//ssao（环境光遮蔽）几何阶段
#version 330 core
layout(location = 0)out vec4 gPositionDepth;
layout(location = 1)out vec3 gNormal;
layout(location = 2)out vec3 gAlbedo;

in VS_OUT
{
	vec3 position;
	vec3 normal;
	vec2 texcoord;
} fs_in;

uniform float near_plane;
uniform float far_plane;

float LinearizeDepth(float depth)
{
	float z = depth * 2.0 - 1.0; //回到NDC,NDC的范围是一个[-1,1]的立方体
	return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}

void main()
{
	gPositionDepth.rgb = fs_in.position;
	//测试gl_FragCoord.z 的值
	gPositionDepth.a = LinearizeDepth(gl_FragCoord.z);
	//gl_FragColor = vec4(vec3(pow(gl_FragColor.z, exp)), 1.0)
	gNormal = normalize(fs_in.normal);
	gAlbedo = vec3(0.95);
}