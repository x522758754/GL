/*
1.根据gBuffer 求当前fragment基本信息
2.求切线空间空间矩阵
3.计算ambient occlusion
*/
#version 330 core
in vec2 texcoord;

uniform sampler2D texture_gPositionDepth;
uniform sampler2D texture_gNormal;
uniform sampler2D texture_gAlbedo;
uniform sampler2D texture_Noise;
uniform vec3 samples[64];
uniform mat4 projection;

//the noise texture over screen based on dimensions divided by noise size
const vec2 noiseScale = vec2(1280.0 / 8.0, 720.0 / 8.0);
//tweak the effect
const int kernelSize = 64;
const float radius = 0.5;
const float bias = 0.025;

out float FragColor;

void main()
{
	vec3 fragPos = texture(texture_gPositionDepth, texcoord).xyz;
	vec3 normal = texture(texture_gNormal, texcoord).rgb;
	vec3 randomVec = texture(texture_Noise, texcoord * noiseScale).rgb;
	//求垂直normal的向量,矫正T，防止有不垂直的情况
	vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(normal, tangent);
	// create TBN change-of-basis matrix: from tangent-space to view-space
	mat3 TBN = mat3(tangent, bitangent, normal);
	// iterate over the sample kernel and calculate occlusion factor
	float occlusion = 0.0;
	for(int i=0; i != kernelSize; ++i)
	{
		//get sample position
		vec3 sample = TBN * samples[i]; //from tagent to view-space
		sample = fragPos + sample * radius;
		//project sample position(to sample texture)(to get position on screen/texture)
		vec4 offset = vec4(sample, 1.0);
		offset = projection * offset; //from view to clip-space
		offset.xyz /= offset.w; //perspective divide
		offset.xyz = offset.xyz * 0.5 + 0.5;
		//get sample depth
		float sampleDepth = texture(texture_gPositionDepth, offset.xy).z; //get depth value of kernel sample
		//range check & accumulate
		float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
		occlusion += (sampleDepth >= sample.z + bias ? 1.0 : 0.0);
	}
	occlusion = 1.0 - (occlusion / kernelSize);
	vec3 result = tangent;
	FragColor = occlusion;
}