#version 330 core

in VS_OUT
{
	vec2 texcoord;
	vec3 lightDir;
	vec3 viewDir;
}fs_in;

uniform sampler2D diffuseTex;
uniform sampler2D normalTex;

out vec4 fragColor;
void main()
{
	vec3 halfDir = normalize(fs_in.lightDir + fs_in.viewDir);
	vec3 color = texture(diffuseTex, fs_in.texcoord);
	vec3 normal = texture(normalTex, fs_in.texcoord);
	float ambient = 0.15;
	float diffuse = max(0, dot(halfDir, normal));
	float spec = pow(diffuse, 32);
	fragColor = color * (ambient + diffuse + spec);
}