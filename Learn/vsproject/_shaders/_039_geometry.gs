#version 330 core
layout(points) in;
// points：绘制GL_POINTS图元时（1）。
// lines：绘制GL_LINES或GL_LINE_STRIP时（2）
// lines_adjacency：GL_LINES_ADJACENCY或GL_LINE_STRIP_ADJACENCY（4）
// triangles：GL_TRIANGLES、GL_TRIANGLE_STRIP或GL_TRIANGLE_FAN（3）
// triangles_adjacency：GL_TRIANGLES_ADJACENCY或GL_TRIANGLE_STRIP_ADJACENCY（6）
layout(triangle_strip, max_vertices = 5) out;

out vec3 fColor;
// points
// line_strip
// triangle_strip

in VS_OUT
{
	vec3 color;
} gs_in[];

// in gl_Vertex
// {
// 	vec4  gl_Position;
// 	float gl_PointSize;
// 	float gl_ClipDistance[];
// } gl_in[];

void build_house(vec4 position)
{
	gl_Position = position + vec4(-0.2, -0.2, 0.0, 0.0);    // 1:左下
	EmitVertex();   
	gl_Position = position + vec4( 0.2, -0.2, 0.0, 0.0);    // 2:右下
	EmitVertex();
	gl_Position = position + vec4(-0.2,  0.2, 0.0, 0.0);    // 3:左上
	EmitVertex();
	gl_Position = position + vec4( 0.2,  0.2, 0.0, 0.0);    // 4:右上
	EmitVertex();
	gl_Position = position + vec4( 0.0,  0.4, 0.0, 0.0);    // 5:顶部
	EmitVertex();
	EndPrimitive();
}

void main()
{
	fColor = gs_in[0].color;
	build_house(gl_in[0].gl_Position);

	// gl_Position = gl_in[0].gl_Position + vec4(-0.1, 0.0, 0.0, 0.0);
	// EmitVertex(); //每次我们调用EmitVertex时，gl_Position中的向量会被添加到图元中来

	// gl_Position = gl_in[0].gl_Position + vec4(0.1, 0.0, 0.0, 0.0);
	// EmitVertex();

	// EndPrimitive(); //当EndPrimitive被调用时，所有发射出的(Emitted)顶点都会合成为指定的输出渲染图元
}