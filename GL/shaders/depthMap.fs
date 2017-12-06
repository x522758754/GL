#version 330 core

in vec4 fragPos;
uniform vec3 lightPos;
uniform float far_plane;

void main()
{
	vec3 distLight2Frag = lightPos - fragPos.xyz;
	float depth = length(distLight2Frag) / far_plane;
	gl_FragDepth = depth;
}