#version 330 core
out float FragColor;

in vec2 texcoord;
uniform sampler2D texture_ssaoInput;

void main()
{
	vec2 texelSize = 1.0 / vec2(textureSize(texture_ssaoInput, 0));
	float result = 0.0;
	for(int x = -2; x < 2; ++x) //length:4
	{
		for(int y = -2; y < 2; ++y)//length:4
		{
			vec2 offset = vec2(float(x), float(y)) * texelSize;
			result += texture(texture_ssaoInput, texcoord + offset).r;
		}
	}
	FragColor = result / (4.0 * 4.0);
}