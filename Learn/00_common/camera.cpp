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
	//����ռ���� (right�� up�� front�� w)
	///				1	0	0	world_camera_x		
	///lookAt =	[	0	1	0	world_camera_y ]
	///				0	0	-1  world_camera_z
	///				0	0	0	1

	//����ռ���� (right�� up�� front�� w)
	///				rx	ux	fx	-dot(r, eye)		
	///lookAt =	[	ry	uy	fy	-dot(u, eye) ]
	///				rz	uz	fz  dot(f, eye)
	///				0	0	0	1
	return glm::lookAt(_position, _position + _front, _up);
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
	//Ϊ�˱������������������pitch��[-89.0,89.0]�ķ�Χ�ڣ�����yaw��[0,360.0]��Χ��

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
	///right-hand����ϵ��
	///OpenGLʹ��������

	///Mpitch(��x����תpitch�õ�����ת����)
	/*	x		y		z
	1		0		0
	0		cos		-sin
	0		sin		cos
	*/
	///Myaw(��y����תyaw�õ�����ת����)
	/*	x		y		z
	cos		0		sin
	0		1		0
	-sin	0		cos
	*/
	///M = Myaw * Mpitch
	/*	x					y					z
		cosyaw				sinpitch*sinyaw		sinyaw*cospitch		
		0					cospitch			-sinpitch
		sinyw				sinpitch*cosyaw		cospitch*cosyaw
	*/

	///world space�£����������ʼ����
	glm::vec3 originalForward = glm::vec3(0, 0, -1);	//�����ʼ����
	glm::vec3 originalUp = glm::vec3(0, 1, 0);			//�����ʼ�Ϸ�

	///ע����Ϊ����������з���(roll)�����Ծ���pitch��yaw���µĳ���(_front)���µ�������(_up)���Լ�ԭʼ������(originalUp)3����������
	///��pitch��roll��worldForward�������_forward
	///��3���������棬��������������(_right)
	///�ɲ�ˣ�������������(_up)

	///�������ǰ��pitch��yaw����forward
	/// forward = M * forward
	_front.x = sin(glm::radians(_pitch)) * cos(glm::radians(_yaw)) * originalForward.z;
	_front.y = -sin(glm::radians(_yaw)) * originalForward.z;
	_front.z = cos(glm::radians(_pitch)) * cos(glm::radians(_yaw)) * originalForward.z;
	_front = glm::normalize(_front);//�����ǰ����

	_right = glm::normalize(glm::cross(_front, originalUp));//����Ҳࣨ���ᣩ
	
	_up = glm::normalize(glm::cross(_right, _front));//����Ϸ�(����)
}

void camera::UpdateEular()
{
	glm::vec3 originalForward = glm::vec3(0, 0, -1);	//�����ʼ����
	glm::vec3 originalUp = glm::vec3(0, 1, 0);			//�����ʼ�Ϸ�
	//�ȼ���pitch
	_front.x = 0;
	_front.y = -sin(glm::radians(_pitch)) * originalForward.z;
	_front.z = cos(glm::radians(_pitch)) * originalForward.z;
	//����yaw
	_front.x = sin(glm::radians(_yaw)) * _front.z;
	_front.y = 1 * _front.y;
	_front.z = cos(glm::radians(_yaw)) * _front.z;

	_front = glm::normalize(_front);//�����ǰ����

	_right = glm::normalize(glm::cross(_front, originalUp));//����Ҳࣨ���ᣩ

	_up = glm::normalize(glm::cross(_right, _front));//����Ϸ�(����)
}
