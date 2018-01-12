//疑问点:两次cross不明白
//疑问重重
#version 330 core

out vec4 FragColor;

in vec3 localPos;
const float PI = 3.14159265359;
uniform samplerCube envCubemap;

void main()
{
	vec3 froward = normalize(localPos);
	vec3 irradiance = vec3(0.0);

	vec3 up = vec3(0.0, 1.0, 0.0);
	vec3 right = cross(up, froward);
	up =  cross(froward, right);

	float sampleDelta = 0.025; //采样精度
	float nrSamples = 0.0;

	//双重积分
	//Lo(p,ϕo,θo)=kdcπ∫2πϕ=0∫12πθ=0Li(p,ϕi,θi)cos(θ)sin(θ)dϕdθ
	//采样沿着法线朝向的上半球光源，两个球形值（横向整圆，纵向半圆）采样,
	//在两个循环中，我们将两个球面坐标转换成三维笛卡尔样本向量，将样本从切线转换到世界空间，
	//并使用此样本向量直接对HDR环境地图进行采样。 我们将每个样品的结果加到辐照度上，最后除以样品的总数，得到平均的辐照度。
	//1。模拟积分，对每一圈采样(横向)
	for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
	{
		//2.模拟积分，对1/4圈采样(纵向)
		//
		for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
		{
			//spherical to cartesian(in tangent space)
			//球形坐标 =》 笛卡尔坐标(切线空间)
			vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
			//tangent space to world
			vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * froward;

			//theta表示N与L的夹角
			//cos(theta),表示NdotL；角度越大，光线越弱
			//sin(theta),高半球区域较小样本采样区域（为了显示其更小的面积区域所占比例，当采样中心在接近采样上方(球顶)时，半球的离散样本区域会变小）
			irradiance += texture(envCubemap, sampleVec).rgb * cos(theta) * sin(theta);
			nrSamples ++;
		}
	}
	//PI 不明确
	irradiance = PI * irradiance * (1.0 / nrSamples);
}