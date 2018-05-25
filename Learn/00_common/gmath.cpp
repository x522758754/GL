#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>


///坐标变换的顺序必须是： 缩放->旋转->平移
///https://blog.csdn.net/zsq306650083/article/details/50561857

/// rigid-body transform: only affect object's orientation(旋转变换产生)and location change(位移产生) X = T(t)R,T(t):translation matrix, R: rotation matrix

/// 以某点为中间,绕某个标准轴旋转angle角度
glm::mat4 getRotateMatrix(const glm::vec3 rotateCenterPoint, const glm::vec3 rotateAxis, float rotateAngle)
{
	glm::vec3 coordOrigin(0, 0, 0);

	///移到坐标原点
	glm::mat4 trans2origin;
	trans2origin = glm::translate(trans2origin, coordOrigin - rotateCenterPoint);

	///绕目标轴旋转
	glm::mat4 rotate;
	rotate = glm::rotate(rotate, glm::radians(rotateAngle), rotateAxis);

	///移回到该点
	glm::mat4 transback;
	transback = glm::translate(transback, rotateCenterPoint - coordOrigin);

	///osg,unity3d是行向量,行向量左乘矩阵 A左乘E = AE
	///opengl采用列向量. 列向量右乘矩阵 A右乘E = EA
	glm::mat4 mat = transback * rotate * trans2origin;

	return mat;
}

/// 任意方向旋转
glm::mat3 getRotateMatrix(const glm::vec3 direction, float rotateAngle)
{
	glm::vec3 normR = glm::normalize(direction); // 让旋转方向为x，后面绕其旋转
	glm::vec3 normS; //y
	if (abs(normR.x) <= abs(normR.y) && abs(normR.x) <= abs(normR.z))
	{
		glm::vec3 S(0, -normR.z, normR.y);
		normS = glm::normalize(S);
	}
	else if (abs(normR.y) <= abs(normR.x) && abs(normR.y) <= abs(normR.z))
	{
		glm::vec3 S(-normR.z, 0, normR.x);
		normS = glm::normalize(S);
	}
	else //(abs(normR.z) < abs(normR.x) && abs(normR.x) < abs(normR.y))
	{
		glm::vec3 S(-normR.y, -normR.x, 0);
		normS = glm::normalize(S);
	}

	glm::vec3 T = glm::cross(normR, normS);
	glm::vec3 normT = glm::normalize(T); //z

	glm::mat3 M(normR, normS, normT); //由xyz标准空间到 rst旋转空间的变换矩阵
	glm::mat4 R4;
	glm::mat3 R = (glm::mat3)glm::rotate(R4, rotateAngle, normR);

	glm::mat3 inversM = glm::transpose(M); //rst -> xyz变换矩阵 ; M是 orthogonal matrix m -1 = m T

	return inversM * R * M;
}

/// 镜像
glm::mat4 getMirrorMatrix()
{
	glm::mat4 mirror;
	///	-1
	///		-1
	///			-1
	glm::vec3 vec(-1, -1, -1);
	mirror = glm::scale(mirror, vec);
	return mirror;
}

/// 缩放
///标准缩放S矩阵
///只在 x/y/z 方向上非均匀缩放
glm::mat3 getScaleMatrix(glm::vec3 vecScale)
{
	glm::vec3 xAxis(1, 0, 0);
	glm::vec3 yAxis(0, 1, 0);
	glm::vec3 zAxis(0, 0, 1);

	xAxis = xAxis * vecScale.x;
	yAxis = yAxis * vecScale.y;
	zAxis = zAxis * vecScale.z;

	glm::mat3 mat(xAxis, yAxis, zAxis);

	return mat;
}

/// 沿着任意三个坐标轴非均匀缩放
glm::mat3 getScaleMatrix(const glm::mat3 mat, const glm::vec3 vecScale)
{
	///因为mat正交, inverse(mat) == transpose(mat)
	glm::mat3 matInverse = glm::transpose(mat);
	glm::mat3 matS = getScaleMatrix(vecScale);
	
	//opengl采用列向量
	return mat * matS * matInverse;
}

/// 在某个方向上缩放
/// https://blog.csdn.net/zsq306650083/article/details/8776168
glm::mat2 getScaleMatrix(glm::vec2 direction, float scale)
{
	///标准坐标系 Sx=1; Sy=1
	///	x	y			x	y
	///	Sx	0		=>	1	0
	///	0	Sy			0	1
	
	///假设N向量为缩放的方向单位向量,缩放因子为k，V向量需要缩放，缩放后V'
	/// V = V⊥Ｎ + V∥Ｎ
	/// V' = V'⊥Ｎ + V'∥Ｎ
	// =>
	/// 因为垂直方向的不受缩放影响， V'⊥Ｎ = V⊥Ｎ = V - V∥Ｎ = V - (V·N）N
	/// 平行方向受缩放影响			 V'∥Ｎ = k * V∥Ｎ
	/// V' = V'⊥Ｎ + V'∥Ｎ = V⊥Ｎ + k * V∥Ｎ = V - V∥Ｎ + k * V∥Ｎ = V + (k-1)* V∥Ｎ = V + (k-1)*(V·N）N
	///当V为基向量x = (1,0） => x' = (1 + (k-1)*Nx*Nx, (k-1)*Nx*Ny)

	glm::vec2 N = glm::normalize(direction);
	glm::vec2 X(1, 0), Y(0, 1);
	glm::vec2 X1(1 + (scale - 1) * N.x * N.x, (scale - 1) * N.x * N.y);
	glm::vec2 Y1((scale - 1) * N.x * N.y, 1 + (scale - 1) * N.y * N.y);

	glm::mat2 mat(X1, Y1);

	return mat;
}

/// 在某个方向上缩放
glm::mat3 getScaleMatrix(glm::vec3 direction, float scale)
{
	//同理
	glm::vec3 N = glm::normalize(direction);
	glm::vec3 X(1, 0, 0), Y(0, 0, 1);
	glm::vec3 X1(1 + (scale - 1) * N.x * N.x, (scale - 1) * N.x * N.y, (scale - 1) * N.x * N.z);
	glm::vec3 Y1((scale - 1) * N.x * N.y, 1 + (scale - 1) * N.y * N.y, (scale - 1) * N.y * N.z);
	glm::vec3 Z1((scale - 1) * N.x * N.z, 1 + (scale - 1) * N.y * N.z, (scale - 1) * N.z * N.z);

	glm::mat3 mat(X1, Y1, Z1);

	return mat;
}

///投影
/*
	投影，意味降维操作，将所有的点拉平到要投影的直线或平面上（针对3d世界）
	正交投影：所有原始点到投影点的直线都相互平行
	透视投影：符合人们心理习惯，即离视点近的物体大，离视点远的物体小，远到极点即为消失，成为灭点。
*/

glm::mat2 getProjMatrix(glm::vec2 direction)
{
	///P(n),，在垂直向量n的轴投影，意味该方向的缩放因子为0
	///=> P(n) = S(n,0)

	return getScaleMatrix(direction, 0);
}

glm::mat3 getProjMatrix(glm::vec3 direction)
{
	///P(n),，在垂直向量n的平面投影，意味该方向的缩放因子为0
	///=> P(n) = S(n,0)

	return getScaleMatrix(direction, 0);
}

/*
----------------------------------切变-------------------------------
	切变时候，角度变化，但是面积或体积不变。也可以理解为坐标轴间的角度变化，造成的扭曲。
*/

/// 矩阵初等变换 :如果B可以由A经过一系列初等变换得到，则称矩阵A与B称为等价
/*
	初等行变换
	定义：所谓数域P上矩阵的初等行变换是指下列3种变换：
	1）以P中一个非零的数乘矩阵的某一行
	2）把矩阵的某一行的c倍加到另一行，这里c是P中的任意一个数
	3）互换矩阵中两行的位置
*/

/// 逆矩阵:设A是数域上的一个n阶方阵，若在相同数域上存在另一个n阶矩阵B，使得： AB=BA=I。 则我们称B是A的逆矩阵，而A则被称为可逆矩阵。
/// 正交矩阵：一个方矩阵，转置矩阵等于其逆矩阵
/*
	求逆矩阵：
	1）高斯－若尔当（初等变换法）
	2）伴随矩阵
	3）如果矩阵是由一系列简单变换组成，M = T(t)R(φ), then M −1 = R(−φ)T(−t).
	4）正交矩阵  M −1 = M T 
*/

/// 四元数旋转
/*
	2d复平面下特殊的复数--旋转数(旋转2D复平面的点)：q = cosθ + isinθ; i^2 = -1
	一般旋转四元数：q = cos(θ/2) + vsin(θ/2); v∈R^3, v = xi + yj + zk; i^2 = -1, j^2 = -1, k^2 = -1,
	特殊旋转四元数(q=cosθ+vsinθ)，只针对向量p(p∈R^3),且 p⊥v
	eulerAngles //由四元数q返回旋转欧拉角度
	angleAxis(float angleRadians, vec3 axis) //创建一个旋转四元数，绕axis轴旋转angle，得到一般旋转四元数q
	slerp(q1, q2, t) //球形插值，通过t值q1向q2之间插值
	quat combined_rotation = second_rotation * first_rotation; //怎样累积两个旋转,将两个四元数相乘即可。顺序和矩阵乘法一致
*/

glm::quat getRotateQuat(glm::vec3 direction, float rotateAngle)
{
	glm::vec3 normDir = glm::normalize(direction);
	return glm::angleAxis(glm::radians(rotateAngle), direction); 
}

// void main()
// {
// 	glm::quat q = getRotateQuat(glm::vec3(2, 0, 0), 45);
// 	glm::mat3 mat = getScaleMatrix(glm::vec3(2, 3, 4));
// 
// 	std::cout << mat[0][0] << std::endl;
// }