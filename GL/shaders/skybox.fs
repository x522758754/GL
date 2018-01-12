#version 330 core

out vec4 FragColor;
in vec3 localPos;
in vec4 clip;

uniform samplerCube envCubemap;

void main()
{
	vec3 viewDir = normalize(localPos);
	vec3 color = texture(envCubemap, viewDir).rgb;
	
	//待测试
	//color = pow(color, vec3(2.2));

	color = color / (color + vec3(1.0)); //environment map's hdr values to default ldr framebuffer
	color = pow(color, vec3(1.0 / 2.2)); //almost all hdr maps are in linear color space by default
										 //so we need to apply gamma correction before writing to the default framebuffer.

	FragColor = vec4(color, 1.0);
}