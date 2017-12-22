#version 330 core

out vec4 FragColor;
in vec2 texcoord;

uniform sampler2D scene;
uniform sampler2D sceneBlur;
uniform bool bloom;
uniform float exposure;
const float gamma = 2.2;

void main()
{
	vec3 sceneColor = texture(scene, texcoord).rgb;
	vec3 blurColor = texture(sceneBlur, texcoord).rgb;
	if(bloom)
		sceneColor += blurColor;
	vec3 result = vec3(1.0) - exp(-sceneColor * exposure); //?
	result = pow(result, vec3(1.0 / gamma));

	FragColor = vec4(sceneColor, 1.0);
}