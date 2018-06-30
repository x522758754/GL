#version 330 core
layout (triangles) in; //以3个三角形的顶点作为输入
layout (triangle_strip, max_vertices=18) out; //我们输入一个三角形，输出总共6个三角形（6*3顶点，所以总共18个顶点）

uniform mat4 shadowMatrices[6]; //光空间变换矩阵的uniform数组

out vec4 FragPos; // FragPos from GS (output per emitvertex)

void main()
{
    for(int face = 0; face < 6; ++face)
    {
        gl_Layer = face; // built-in variable that specifies to which face we render.
        for(int i = 0; i < 3; ++i) // for each triangle's vertices
        {
            FragPos = gl_in[i].gl_Position;
            gl_Position = shadowMatrices[face] * FragPos;
            EmitVertex();
        }    
        EndPrimitive();
    }
}