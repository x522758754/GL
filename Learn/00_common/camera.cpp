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
	///ȡ��������ϵ������������λ�����������������������ϵ��λ�ã�����view_tanslate_inverse���󣨽����������ռ�=������ռ䣩
	/// view_tanslate_inverse (cameraPos => worldPos)
	///			x  y  z  w
	///x����	1, 0, 0, _position.x
	///y����	0, 1, 0, _position.y
	///z����	0, 0, 1, _position.z
	///�������	0, 0, 0, 1
	///ע��	w����������ϵ��ԭ�㣬����������ϵ�����λ��,
	///		xyz��������ϵ�ı�׼������(�໥��ֱ������Ϊ1),����������ϵ����������

	/// test
	float scale = 2.0f;
	float rate =  1 / scale;
	glm::mat4 view_scale_inverse = glm::mat4(
		glm::vec4(scale,	0,				0,			0), ///x
		glm::vec4(0,		scale,			0,			0), ///y
		glm::vec4(0,		0,				scale,		0), ///z
		glm::vec4(0,		0,				0,			1)  ///w
	);

	glm::mat4 view_tanslate_inverse = glm::mat4(
		glm::vec4(1,			0,				0,			0), ///x
		glm::vec4(0,			1,				0,			0), ///y
		glm::vec4(0,			0,				1,			0), ///z
		glm::vec4(_position.x, _position.y, _position.z,	1)	///w
	);

	///ͬ��,����view_tanslate_inverse����(��worldPos => cameraPos)
	///ע���˴���frontΪz��������������ϵ���� - front��Ϊ��������ϵ, ��openglͳһ����������ϵ��������ɫ���еľ�����㣬���Դ˴��� -_front
	glm::mat4 view_rotation_inverse = glm::mat4(	
		glm::vec4(_right.x,		_right.y,	_right.z,	0), ///x
		glm::vec4(_up.x,		_up.y,		_up.z,		0), ///y
		glm::vec4(-_front.x,	-_front.y,	-_front.z,	0), ///z
		glm::vec4(0,			0,			0,			1)  ///w
	);

	///ֱ�ӹ�����view_tanslate_inverse����ϵ(��worldPos => cameraPos)
	glm::mat4 view_r_t_inverse = glm::mat4(	
		glm::vec4(_right.x,		_right.y,		_right.z,		0),
		glm::vec4(_up.x,		_up.y,			_up.z,			0),
		glm::vec4(-_front.x,	-_front.y,		-_front.z,		0),
		glm::vec4(_position.x,	_position.y,	_position.z,	1)
	);

	

	///ֱ�ӹ�����view����ϵ (��worldPos => cameraPos)
	glm::mat4 view = glm::mat4(
		glm::vec4(_right.x * rate,						_up.x * rate,						-_front.x * rate,					0),
		glm::vec4(_right.y * rate,						_up.y * rate,						-_front.y * rate,					0),
		glm::vec4(_right.z * rate,						_up.z * rate,						-_front.z * rate,					0),
		glm::vec4(-glm::dot(_position, _right), -glm::dot(_position, _up), glm::dot(_position, _front), 1)
	);

	glm::mat4 view1 = glm::inverse(view_scale_inverse * view_tanslate_inverse * view_rotation_inverse); // (MN)-1 = N-1 M-1
	glm::mat4 view2 = glm::inverse(view_r_t_inverse);

	//s:side -> right
	//u:up -> up
	//f:forward -> front
	glm::mat4 view_3 = glm::lookAt(_position, _position + _front, _up);
	
	return view1;
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
		sinyw				sinpitch*cosyaw		cosyaw*cospitch
	*/

	///�ȼ���Mrotate
	///Mrotate = Myaw * Mpitch
	///��forwardΪ Mrotate * ��forward


	///world space�£����������ʼ����
	glm::vec3 originalForward = glm::vec3(0, 0, -1);	//�����ʼ����
	glm::vec3 originalUp = glm::vec3(0, 1, 0);			//�����ʼ�Ϸ�

	///ע����Ϊ����������з���(roll)�����Ծ���pitch��yaw���µĳ���(_front)���µ�������(_up)���Լ�ԭʼ������(originalUp)3����������
	///��pitch��roll��worldForward�������_forward
	///��3���������棬��������������(_right)
	///�ɲ�ˣ�������������(_up)

	///�������ǰ��pitch��yaw����forward
	/// forward = M * forward
	_front.x = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch)) * originalForward.z;
	_front.y = -sin(glm::radians(_pitch)) * originalForward.z;
	_front.z = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch)) * originalForward.z;
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

	glm::vec3 newRight = glm::cross(_front, _up);
}
