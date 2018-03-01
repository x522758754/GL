#version 330 core

in VS_OUT
{
	vec3 position;
	vec3 normal;
	vec2 texcoord;
	vec3 color;
} fs_in;

uniform sampler2D texture_diffuse1;
//uniform sampler2d texAlpha;
//uniform float desaturate;
out vec4 fragColor;

const float desaturate = 0;

void main()
{
	//vec4 texColor = texture(texture_diffuse1, fs_in.texcoord);
	// texColor.a = texture(texAlpha, fs_in.texcoord).r;
	// if(texColor.a < 0.5)
	// 	discard;
	//是否褪色 => 灰色
	//texColor.rgb = mix(texColor.rgb, dot(texColor.rgb, vec3(0.3, 0.59, 0.11)), desaturate);
	fragColor = vec4(fs_in.color, 1.0); //vec4(1.0, 1.0, 1.0, 1.0); //
}