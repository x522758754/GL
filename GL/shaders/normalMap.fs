#version 330 core

in VS_OUT
{
	vec3 normal;
	vec2 texcoord;
	vec3 lightDir;
	vec3 viewDir;
}fs_in;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;

out vec4 fragColor;

void main()
{
	vec3 halfDir = normalize(fs_in.lightDir + fs_in.viewDir);
	vec3 color = texture(texture_diffuse1, fs_in.texcoord).rgb;
	vec3 normal = texture(texture_normal1, fs_in.texcoord).rgb;
	float ambient = 0.15;
	float diffuse = max(0, dot(halfDir, normal));
	float spec = pow(diffuse, 8);

	fragColor = vec4(color * (ambient + diffuse + spec), 1.0);
}