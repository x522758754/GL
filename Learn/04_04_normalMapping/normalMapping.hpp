#include "app.h"
#include "shader.h"
#include "Utils.h"
#include "Model.h"
//矩阵级联
///可以通过矩阵相乘来将几个矩阵变换的效果合并起来
///两个矩阵相乘的顺序是很重要的，与标量的相乘不同，矩阵相乘的顺序是不能交换的。如果我们将两个矩阵的顺序交换的话，得到的结果就会是，先对飞碟进行平移，然后将它绕世界原点进行旋转。

///使用矩阵的好处之一是如果你使用3个相互垂直（或非线性）的轴定义了一个坐标空间，你可以用这3个轴外加一个平移向量来创建一个矩阵，并且你可以用这个矩阵乘以任何向量来将其变换到那个坐标空间。
///例A空间的坐标轴矩阵Ma,有个向量va，B空间有个坐标轴矩阵Mb，假设向量vb是va在B空间的表示，则vb = Mb * va

///假设在标准基坐标系中有一个向量v(参见1.2.1 节)，这个坐标系的坐标轴用Ex、Ey和Ez描述。此外，还有另外一个坐标系，用任意基向量Fx、Fy和Fz(这些向量定不能共面，也就是说，|Fx, Fy, Fz|≠0)来描述这个坐标系,
///现在，如何用Fx、Fy和Fz来唯地描述向量v ?
///	Fw = (Fx Fy Fz)w = v
///	w = F^-1 v


//法线贴图
///这会是一种偏蓝色调的纹理（你在网上找到的几乎所有法线贴图都是这样的）。
///这是因为所有法线的指向都偏向z轴（0, 0, 1）这是一种偏蓝的颜色。
///法线向量从z轴方向也向其他方向轻微偏移，颜色也就发生了轻微变化，这样看起来便有了一种深度。
///注：法线贴图只能模拟光照带来的明暗, 不能隐藏被遮挡的背面
//视差贴图
///可以让背面被遮挡住的部分完全不显示出来，除此之外还能在一定范围内调整砖块凹凸的程度。
///视差贴图也只是模拟作假，并没有真的改变模型表面

//OpenGL读取的纹理的y（或V）坐标和纹理通常被创建的方式相反。

//切线空间
///法线贴图中的法线向量在切线空间中，法线永远指着正z方向。
///切线空间是位于三角形表面之上的空间：法线相对于单个三角形的本地参考框架。
///它就像法线贴图向量的本地空间；它们都被定义为指向正z方向，无论最终变换到什么方向。
///使用一个特定的矩阵我们就能将本地/切线空间中的法线向量转成世界或视图坐标，使它们转向到最终的贴图表面的方向。

//TBN矩阵 （不考虑位移直接只考虑方向）:世界空间到切线空间 (=>利用正交矩阵的特性求出切线空间到世界空间，)
///已知：模型空间（Object Space	）下的tangent、normal向量，Matrix_model矩阵（将顶点坐标变换到世界坐标）
///将normal变换到世界空间,矩阵Matrix_model_normal = transpose(inverse(Matrix_model))
///求出world_tangent = Matrix_model * normal, world_normal = Matrix_model_normal * normal
///因为TBN矩阵的顺序为tangent、bitangent、normal 对应x、y、z的顺序，考虑bitangent的方向问题，所以world_bitangent = cross(world_normal, world_tangent)
///至此，在世界坐标系下，由TBN构建的矩阵，可以将向量由世界空间变换到切线空间

//TBN逆矩阵：切线空间到世界空间
///tangent在切线空间下的向量，world在世界空间下的向量
///设： tangent = TBN world
///有： TBN^-1 tangent = TBN ^-1 TBN world 
///		TBN^-1 tangent = world

//view矩阵 (right、 up、 front):世界空间到相机空间 (wc:相机在世界坐标系的坐标,相机需要先平移到原点)
///															rx	ux	fx	0			1	0	0	-wcx
///Matrix_lookAt =	 Matrix_rotation * Matrix_translate = [	ry	uy	fy	0	] * [	0	1	0	-wcy	]
///															rz	uz	fz	0			0	0	1	-wcz
///															rw	uw	fw	1			0	0	0	w
///Matrix_translate：先将相机在世界空间坐标系下的位置移到原点（注：目的是以相机为原点进行接下来的旋转，参考real-time书籍）
///Matrix_rotation: 以相机在世界坐标系下right、 up、 front构建的矩阵；可以将向量由世界空间变换到相机空间

class normalMapping : public app
{
	Shader shader;
	GLuint tex_diff, tex_normal, tex_spec;
	GLuint planeVAO, cubeVAO;
	glm::vec3 lightPos;

	Model m;

	void start()
	{	
		shader = Shader("_shaders/_0404_normalMapping.vs", "_shaders/_0404_normalMapping.fs");

		tex_diff = Utils::loadTexture("_texs/brickwall.jpg");
		tex_normal = Utils::loadTexture("_texs/brickwall_normal.jpg");

		m = Model("_models/Cyborg/Cyborg.obj");

		shader.use();
		shader.setInt("texture_diffuse1", 0);
		shader.setInt("texture_normal1", 1);

		cam->set_position(glm::vec3(0, 0, 3));
		lightPos = glm::vec3(2, 2, 2);
		planeVAO = 0;

		glEnable(GL_DEPTH_TEST);
		glCullFace(GL_BACK);
	}

	void render(double time)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.1f, 0.1f, 0.1f, 1.f);

		glm::mat4 model, view, projection;
		view = cam->GetViewMatrix();
		projection = glm::perspective(glm::radians(cam->get_fovy()), 1.f * cfg.scrWidth / cfg.scrHeight, 0.1f, 100.f);
		shader.setMat4("model", model);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		shader.setVec3("lightPos", lightPos);
		shader.setVec3("viewPos", cam->get_position());

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex_diff);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, tex_normal);

		drawPlane();

		model = glm::mat4();
		model = glm::translate(model, lightPos);
		model = glm::translate(model, glm::vec3(0.1f));
		shader.setMat4("model", model);

		drawCube(&cubeVAO);

		model = glm::mat4();
		shader.setMat4("model", model);

		m.Draw(shader);



	}

	void drawPlane()
	{
		if (planeVAO == 0)
		{
			// positions
			glm::vec3 pos1(-1.0f, 1.0f, 0.0f);
			glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
			glm::vec3 pos3(1.0f, -1.0f, 0.0f);
			glm::vec3 pos4(1.0f, 1.0f, 0.0f);

			// texture coordinates
			glm::vec2 uv1(0.0f, 1.0f);
			glm::vec2 uv2(0.0f, 0.0f);
			glm::vec2 uv3(1.0f, 0.0f);
			glm::vec2 uv4(1.0f, 1.0f);

			// normal vector
			glm::vec3 nm(0.0f, 0.0f, 1.0f);

			// calculate tangent/bitangent vectors of both triangles
			glm::vec3 tangent1;
			glm::vec3 tangent2;
			// triangle 1
			// ----------
			glm::vec3 edge1 = pos2 - pos1;
			glm::vec3 edge2 = pos3 - pos1;
			glm::vec2 deltaUV1 = uv2 - uv1;
			glm::vec2 deltaUV2 = uv3 - uv1;

			GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

			tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
			tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
			tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
			tangent1 = glm::normalize(tangent1);

			// triangle 2
			// ----------
			edge1 = pos3 - pos1;
			edge2 = pos4 - pos1;
			deltaUV1 = uv3 - uv1;
			deltaUV2 = uv4 - uv1;

			f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

			tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
			tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
			tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
			tangent2 = glm::normalize(tangent2);

			float vertice[] = {
				// positions            // normal         // texcoords  // tangent                          // bitangent
				pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, 
				pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, 
				pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, 

				pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z,
				pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, 
				pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, 
			};

			glGenVertexArrays(1, &planeVAO);
			glBindVertexArray(planeVAO);

			GLuint VBO;
			glGenBuffers(1, &VBO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertice), vertice, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(0 * sizeof(float))); // 3 + 3 + 2 + 3
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float))); // 3 + 3 + 2 + 3
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float))); // 3 + 3 + 2 + 3
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float))); // 3 + 3 + 2 + 3
			glEnableVertexAttribArray(3);
		}

		glBindVertexArray(planeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
};