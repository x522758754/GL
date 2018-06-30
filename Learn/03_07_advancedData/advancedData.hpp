#include "app.h"
#include "shader.h"
#include "Utils.h"

///OpenGL中的缓冲只是一个管理特定内存块的对象，没有其它更多的功能了

///调用glBufferData函数来填充缓冲对象所管理的内存，这个函数会分配一块内存，并将数据添加到这块内存中。如果我们将它的data参数设置为NULL，那么这个函数将只会分配内存，但不进行填充。

///glBufferSubData，填充缓冲的特定区域。这个函数需要一个缓冲目标、一个偏移量、数据的大小和数据本身作为它的参数。

///将数据导入缓冲的另外一种方法是，请求缓冲内存的指针，直接将数据复制到缓冲当中。通过调用glMapBuffer函数，OpenGL会返回当前绑定缓冲的内存指针
/// glBindBuffer(GL_ARRAY_BUFFER, buffer);
/// // 获取指针
/// void *ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
/// // 复制数据到内存
/// memcpy(ptr, data, sizeof(data));
/// // 记得告诉OpenGL我们不再需要这个指针了
/// glUnmapBuffer(GL_ARRAY_BUFFER);

///顶点着色器变量
//gl_Position
//gl_PointSize 
///通过OpenGL的glPointSize函数来设置渲染出来的点的大小，但我们也可以在顶点着色器中修改这个值。
///glEnable(GL_PROGRAM_POINT_SIZE);
//gl_VertexID （只读）
///我们只能对它进行读取,储存了正在绘制顶点的当前ID。当（使用glDrawElements）进行索引渲染的时候，这个变量会存储正在绘制顶点的当前索引。
///当（使用glDrawArrays）不使用索引进行绘制的时候，这个变量会储存从渲染调用开始的已处理顶点数量。

///片段着色器
//gl_FragCoord (只读)
///gl_FragCoord的z分量等于对应片段的深度值,gl_FragCoord的x和y分量是片段的窗口空间(Window-space)坐标，其原点为窗口的左下角
//gl_FrontFacing （只读）
///我们提到OpenGL能够根据顶点的环绕顺序来决定一个面是正向还是背向面。如果我们不（启用GL_FACE_CULL来）使用面剔除，那么gl_FrontFacing将会告诉我们当前片段是属于正向面的一部分还是背向面的一部分。
///gl_FrontFacing变量是一个bool，如果当前片段是正向面的一部分那么就是true，否则就是false。
//gl_FragDepth
///使用它来在着色器内设置片段的深度值,直接写入一个0.0到1.0之间的float值
///如果着色器没有写入值到gl_FragDepth，它会自动取用gl_FragCoord.z的值。
///由我们自己设置深度值有一个很大的缺点，只要我们在片段着色器中对gl_FragDepth进行写入，OpenGL就会（像深度测试小节中讨论的那样）禁用所有的提前深度测试(Early Depth Testing)。
///它被禁用的原因是，OpenGL无法在片段着色器运行之前得知片段将拥有的深度值，因为片段着色器可能会完全修改这个深度值。
///从OpenGL 4.2起，我们仍可以对两者进行一定的调和，在片段着色器的顶部使用深度条件(Depth Condition)重新声明gl_FragDepth变量：layout (depth_<condition>) out float gl_FragDepth;
///condition(值):
///any	默认值。提前深度测试是禁用的，你会损失很多性能
///greater	你只能让深度值比gl_FragCoord.z更大
///less	你只能让深度值比gl_FragCoord.z更小
///unchanged	如果你要写入gl_FragDepth，你将只能写入gl_FragCoord.z的值
//接口块

//Uniform缓冲对象
///它允许我们定义一系列在多个着色器中相同的全局Uniform变量。
///当使用Uniform缓冲对象的时候，我们只需要设置相关的uniform一次。当然，我们仍需要手动设置每个着色器中不同的uniform。并且创建和配置Uniform缓冲对象会有一点繁琐。
//这个基准对齐量是使用std140布局,每4个字节将会用一个N来表示
///类型	布局规则
///标量，比如int和bool	每个标量的基准对齐量为N。
///向量	2N或者4N。这意味着vec3的基准对齐量为4N。
///标量或向量的数组	每个元素的基准对齐量与vec4的相同。
///矩阵	储存为列向量的数组，每个向量的基准对齐量与vec4的相同。
///结构体	等于所有元素根据规则计算后的大小，但会填充到vec4大小的倍数。

class advancedData : public app
{
	void start()
	{

	}

	void render(double time)
	{

	}

	void shutdown()
	{

	}
};