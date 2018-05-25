#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;
uniform sampler2D texture1;

void main()
{
	vec4 mixTex = mix(texture(ourTexture, TexCoord), texture(texture1, TexCoord), 0.2);
	FragColor= mixTex;
}