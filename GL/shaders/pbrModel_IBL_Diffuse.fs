#version 330 core

out vec4 FragColor;

in VS_OUT
{
	vec3 worldPos;
	vec3 normal;
	vec2 texcoord;
} fs_in;

const int N_LIGHT_SIZE = 4;
const float PI = 3.14159265359;

uniform vec3 lightPositions[N_LIGHT_SIZE];
uniform vec3 lightColors[N_LIGHT_SIZE];

uniform vec3 cameraPos;
uniform vec3 albedo;
uniform float metallic;
uniform float roughness;
uniform float ao;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;
uniform samplerCube irradianceMap;

//fresnel equation, calculate the ratio between surface reflects light(specular reflection) and refracts light(diffuse reflection)
vec3 FresnelSchlick(float cosTheta, vec3 F0, float roughness)
{
	//The Fresnel-Schlick approximation expects a F0 parameter: 
	//0°入射角反射(垂直)观察时，多少光线会被反射
	//in the pbr metallic workflow, assumption: dielectric surface: 0.04, metalloc surface: hr albedo value.

	//return ratio of light that gets reflected on a surface(Ks)
	//return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
	
	//add roughness term
	return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

// the Trowbridge-Reitz GGX normal distribution function
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	// Based on observations by Disney and adopted by Epic Games the lighting looks more correct,
	// squaring the roughness in both geometry and normal distribution function
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0);
	float NdotH2 = NdotH * NdotH;

	float numerator = a2;

	float denominator = NdotH2 * (a2 - 1) + 1;
	denominator = PI * denominator * denominator;

	return numerator / denominator;
}

//Gsub:Schlick-Beckmann approximation known as Schlick-GGX
float GeometrySchlickGGX(float Ndot, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;
	float numerator = Ndot;
	float denominator = Ndot * (1.0 - k) + k;

	return numerator / denominator;
}

//Smith's method with Schlick-GGX: G(n,v,l,k)=Gsub(n,v,k)Gsub(n,l,k)
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotL = max(dot(N, L), 0.0);
	float NdotV = max(dot(N, V), 0.0);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);

	return ggx1 * ggx2;
}

//疑问
// Easy trick to get tangent-normals to world-space to keep PBR code simplified.
// Don't worry if you don't get what's going on; you generally want to do normal 
// mapping the usual way for performance anways; I do plan make a note of this 
// technique somewhere later in the normal mapping tutorial.
vec3 GetNormalFromMap()
{
	vec3 tangentNormal = texture(normalMap, fs_in.texcoord).rgb;
	tangentNormal * 2.0 - 1.0;

	vec3 Q1 = dFdx(fs_in.worldPos);
	vec3 Q2 = dFdy(fs_in.worldPos);
	vec2 st1 = dFdx(fs_in.texcoord);
	vec2 st2 = dFdy(fs_in.texcoord);

	vec3 N = normalize(fs_in.normal);
	vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
	vec3 B = -normalize(cross(N,T));
	mat3 TBN = mat3(T, B, N);

	return normalize(TBN * tangentNormal);
}


void main()
{
	vec3 normal = normalize(fs_in.normal);
	vec3 viewDir = normalize(cameraPos - fs_in.worldPos);
	//albedo = pow(albedo, vec3(2.2));

	// texture pbr
	// albedo = pow(texture(albedoMap, fs_in.texcoord).rgb, vec3(2.2));
	// normal = GetNormalFromMap();
	// metallic = texture(metallicMap, fs_in.texcoord).r;
	// roughness = texture(roughnessMap, fs_in.texcoord).r;
	// ao = texture(aoMap, fs_in.texcoord).r;
	vec3 irradiance = texture(irradianceMap, normal).rgb;

	vec3 F0 = vec3(0.04); //大多数电介质表面使用0.04作为基础反射率足够好
	//因为金属表面会吸收所有折射光线而没有漫反射，所以我们可以直接使用表面颜色纹理来作为它们的基础反射率
	F0 = mix(F0, albedo, metallic);

	//reflectance equation
	//Lo(p,ωo)=∫Ω(kdc/π+ksDFG/4(ωo.n)(ωi.n))Li(p,ωi)n.ωidωi
	vec3 Lo = vec3(0.0);//outgoing radiance -- 物体表面的辐射(折射+反射) 
	for(int i=0; i < N_LIGHT_SIZE; ++i)
	{
		vec3 lightDir = normalize(lightPositions[i] - fs_in.worldPos);
		vec3 halfWayDir = normalize(lightDir + viewDir);

		float distance = length(lightPositions[i] - fs_in.worldPos);
		float attenuation = 1.0 / (distance * distance);
		vec3 radiance = lightColors[i] * attenuation; //
		//calc term D(NDF) 

		float cosTheta = max(0.0, dot(halfWayDir, viewDir)); //there n,v or h,v
		vec3 F = FresnelSchlick(cosTheta, F0, roughness);
		float D = DistributionGGX(normal, halfWayDir, roughness);
		float G = GeometrySmith(normal, viewDir, lightDir, roughness);
		//Cook-Torrance BRDF: fr=kdflambert+ksfcook−torrance
		vec3 numerator = D * F * G;
		float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0);
		//prevent a divide by zero
		vec3 specular = numerator / max(0.001, denominator);
		vec3 Ks = F; //入射光线中被反射的比率
		vec3 Kd = vec3(1.0) - Ks; //根据能量守恒,入射光线中被折射的比率
		//对于金属表面，所有的折射光都会被直接吸收而不会散开
		Kd *= 1.0 - metallic;
		// add to outgoing radiance Lo
		//it's important to note that we left Ks out the equation,Ks equals F,
		//there's no need to multiply by the Frenel again
		float NdotL = max(dot(normal, lightDir), 0.0);
		Lo += (Kd * albedo / PI + specular) * radiance * NdotL;
	}
	// ambient lighting (we now use IBL as the ambient term)
	//vec3 ambient = vec3(0.002);
	vec3 Ks = FresnelSchlick(max(0, dot(normal, viewDir)), F0, roughness);
	vec3 Kd = 1.0 - Ks;
	Kd *= 1.0 - metallic;
	vec3 ambient = Kd * irradiance * albedo * ao;

	vec3 color = ambient + Lo;
	//hdr rendering
	color = color / (color + vec3(1.0));
	//gamma correct
	color = pow(color, vec3(1.0/2.2)); 

	FragColor = vec4(color, 1.0);
}