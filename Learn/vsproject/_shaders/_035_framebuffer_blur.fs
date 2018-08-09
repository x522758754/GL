#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

const float offset = 1.0 / 300.0;  

void main()
{ 
	//Post-processing
	//kernel(核) / convolution matrix (卷积矩阵)
	//它的中心为当前像素

	//1    2    1
	//2    4    2    / 16
	//1....2....1
	//这由于所有值的和是16，所以直接返回合并的采样颜色将产生非常亮的颜色，所以我们需要将核的每个值都除以16。最终的核数组将会是：
	//你在网上找到的大部分核将所有的权重加起来之后都应该会等于1，
	//如果它们加起来不等于1，这就意味着最终的纹理颜色将会比原纹理值更亮或者更暗了。

    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // 左上
        vec2( 0.0f,    offset), // 正上
        vec2( offset,  offset), // 右上
        vec2(-offset,  0.0f),   // 左
        vec2( 0.0f,    0.0f),   // 中
        vec2( offset,  0.0f),   // 右
        vec2(-offset, -offset), // 左下
        vec2( 0.0f,   -offset), // 正下
        vec2( offset, -offset)  // 右下
    );

    //注意：1/16=0; 1.0 / 16 != 0
	float kernel[9] = float[](
         1.0/16,  2.0/16,  1.0/16,
         2.0/16,  4.0/16,  2.0/16,
         1.0/16,  2.0/16,  1.0/16
    );

    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i] ;
    FragColor = vec4(col, 1.0);
    //FragColor = texture(screenTexture, TexCoords.st);
}