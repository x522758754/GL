#include "camera.h"


camera::camera(glm::vec3 position) :
	_pitch(0),
	_yaw(0),
	_zoomSpeed(5),
	_rotateSpeed(0.1),
	_fovy(45),
	_position(position)
{
	UpdateEular();
}

camera::~camera()
{

}

float camera::get_fovy()
{
	return _fovy;
}

glm::vec3 camera::get_position()
{
	return _position;
}

glm::vec3 camera::get_front()
{
	return _front;
}

glm::mat4 camera::GetViewMatrix()
{

	///取世界坐标系的三个三个单位正交向量和相机在世界坐标系的位置，构造view矩阵（将坐标从世界空间 =》 相机空间）
	///view矩阵代表，以相机为参考的坐标系
	
	///首先将相机位置位移世界坐标系原点，得到平移矩阵view_t
	///之后其他世界坐标系的位置使用此矩阵，则是平移到相对相机位置为参考系（相机位置为坐标系原点）的空间
	glm::mat4 view_t = glm::mat4(
		///		  x				y				z			w		
		glm::vec4(1,			0,				0,			0),
		glm::vec4(0,			1,				0,			0),
		glm::vec4(0,			0,				1,			0),
		glm::vec4(-_position.x, -_position.y, -_position.z, 1)
	);

	///以相机为参考（即相机位置坐标系原点），旋转构造旋转矩阵
	///注：旋转坐标系（旋转矩阵）都是以原点中心进行旋转
	///注：如果世界坐标未转换到view_t所在的空间，直接使用此矩阵结果是，该坐标是以世界坐标系为原点进行旋转。
	///注：此处以front为z轴则是左手坐标系，以 - front则为右手坐标系, 用opengl统一用右手坐标系（包含着色器中的矩阵计算，所以此处用 -_front
	glm::mat4 view_r = glm::mat4(
		///		  x			y		z			w		
		glm::vec4(_right.x, _up.x, -_front.x,	0),
		glm::vec4(_right.y, _up.y, -_front.y,	0),
		glm::vec4(_right.z, _up.z, -_front.z,	0),
		glm::vec4(0,		0,		0,			1)
	);

	///直接构建的view坐标系
	glm::mat4 view_r_t = glm::mat4(
		///		  x								y							z							w		
		glm::vec4(_right.x,						_up.x,						-_front.x,					0),
		glm::vec4(_right.y,						_up.y,						-_front.y,					0),
		glm::vec4(_right.z,						_up.z,						-_front.z,					0),
		glm::vec4(-glm::dot(_position, _right), -glm::dot(_position, _up), glm::dot(_position, _front), 1)
	);

	glm::mat4 view = view_r * view_t;

	glm::mat4 view_s = glm::lookAt(_position, _position + _front, _up);
	
	return view;
}

void camera::handleEular(Camera_Eular direction)
{
	switch (direction)
	{
	case Up:
		_pitch += 10;
		break;
	case Down:
		_pitch -= 10;
		break;
	case Left:
		_yaw -= 15;
		break;
	case Right:
		_yaw += 15;
		break;
	default:
		break;
	}

	UpdateEular();
}

void camera::handleKey(Camera_Movement direction, float deltaTime)
{
	float move = _zoomSpeed * deltaTime;
	switch (direction)
	{
	case FORWARD:
		_position += _front * move;
		break;
	case BACKWARD:
		_position -= _front * move;
		break;
	case LEFT:
		_position -= _right * move;
		break;
	case RIGHT:
		_position += _right * move;
		break;
	default:
		break;
	}
}

void camera::handleMouse(float x, float y)
{
	//为了避免出现万向锁，控制pitch在[-89.0,89.0]的范围内，控制yaw在[0,360.0]范围内

	_pitch += y * _rotateSpeed;
	if (abs(_pitch) > 89) 
		_pitch = _pitch > 0 ? 89 : -89;

	_yaw -= x * _rotateSpeed;

	UpdateEular();
}

void camera::handleWheel(float y)
{
	_fovy -= y;
	
	_fovy = fmin(fmax(_fovy, 1.f), 75.f);
}

void camera::set_position(glm::vec3 pos)
{
	_position = pos;
}

void camera::updateCameraAttr()
{
	///right-hand坐标系下
	///OpenGL使用列向量

	///Mpitch(绕x轴旋转pitch得到的旋转矩阵)
	/*	x		y		z
		1		0		0
		0		cos		-sin
		0		sin		cos
	*/
	///Myaw(绕y轴旋转yaw得到的旋转矩阵)
	/*	x		y		z
		cos		0		sin
		0		1		0
		-sin	0		cos
	*/
	///M = Myaw * Mpitch
	/*	x					y					z
		cosyaw				sinpitch*sinyaw		sinyaw*cospitch		
		0					cospitch			-sinpitch
		sinyw				sinpitch*cosyaw		cosyaw*cospitch
	*/

	///先计算Mrotate
	///Mrotate = Myaw * Mpitch
	///新forward为 Mrotate * 老forward


	///world space下：定义相机初始属性
	glm::vec3 originalForward = glm::vec3(0, 0, -1);	//相机初始朝向
	glm::vec3 originalUp = glm::vec3(0, 1, 0);			//相机初始上方

	///注：因为相机不允许有翻滚(roll)，所以经过pitch和yaw后，新的朝向(_front)和新的上向量(_up)、以及原始上向量(originalUp)3个向量共面
	///由pitch和roll、worldForward可以求得_forward
	///由3个向量共面，可以求得相机右轴(_right)
	///由叉乘，可求得相机上轴(_up)

	///由相机当前的pitch和yaw计算forward
	/// forward = M * forward
	_front.x = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch)) * originalForward.z;
	_front.y = -sin(glm::radians(_pitch)) * originalForward.z;
	_front.z = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch)) * originalForward.z;
	_front = glm::normalize(_front);//相机当前朝向

	_right = glm::normalize(glm::cross(_front, originalUp));//相机右侧（右轴）
	
	_up = glm::normalize(glm::cross(_right, _front));//相机上方(上轴)
}

void camera::UpdateEular()
{
	glm::vec3 originalForward = glm::vec3(0, 0, -1);	//相机初始朝向
	glm::vec3 originalUp = glm::vec3(0, 1, 0);			//相机初始上方
	//先计算pitch
	_front.x = 0;
	_front.y = -sin(glm::radians(_pitch)) * originalForward.z;
	_front.z = cos(glm::radians(_pitch)) * originalForward.z;
	//计算yaw
	_front.x = sin(glm::radians(_yaw)) * _front.z;
	_front.y = 1 * _front.y;
	_front.z = cos(glm::radians(_yaw)) * _front.z;

	_front = glm::normalize(_front);//相机当前朝向

	_right = glm::normalize(glm::cross(_front, originalUp));//相机右侧（右轴）

	_up = glm::normalize(glm::cross(_right, _front));//相机上方(上轴)

	glm::vec3 newRight = glm::cross(_front, _up);
}
