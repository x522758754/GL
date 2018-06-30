#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2DMS screenTexture;
uniform int samples;

void main()
{ 
	// for(int i=0; i != samples; ++i)
 //    	FragColor += texelFetch(screenTexture, TexCoords, i);

 //    FragColor /= samples;
}