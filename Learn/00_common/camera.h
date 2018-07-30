#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

enum Camera_Eular
{
	Left,	//yaw 
	Right,	//yaw
	Up,		//pitch
	Down	//pitch

};

class camera
{
public:
	camera(glm::vec3 position = glm::vec3(0, 0, 0));

	~camera();

	/// ��ǰ������
	float get_pitch();

	/// ��ǰƫ����
	float get_yaw();

	/// ��ǰ��Ұ�Ƕ�
	float get_fovy();

	/// �����ǰλ��
	glm::vec3 get_position();

	/// �����ǰ����
	glm::vec3 get_front();

	/// ��ǰ�Ӿ���
	glm::mat4 GetViewMatrix();

	/// ���Թ̶�ŷ����ƫת
	void handleEular(Camera_Eular direction);

	/// ��������
	void handleKey(Camera_Movement direction, float deltaTime);

	/// ��괦��
	void handleMouse(float x, float y);

	/// ���ᴦ��
	void handleWheel(float y);

	void set_position(glm::vec3 pos);

private:
	///������ʹ�ù�ת��(roll),�ڴ�ʹ�ù̶���������
	///const glm::vec3 _worldup = glm::vec3(0, 1, 0); 

	//ŷ����(Euler Angle) ��������ϵ
	///����������,����pitch��[-89.0,89.0]�ķ�Χ�ڣ�����yaw��[0,360.0]��Χ��
	float _pitch;	///������(��λ:��) ͷ��x��
	float _yaw;		///ƫ����(��λ:��) ͷ��y��
	float _roll;	///������(��λ:��) ͷ��z��;�˴�������

	//�������
	float _zoomSpeed;	///��������ٶ�
	float _rotateSpeed;	///���ת���ٶ�(pitchSpeed��yawSpeed���ô��ٶ�)
	float _fovy;		///���y������Ұ�Ƕ�,��׶��Ĵ�ֱ�����ϵĽǶ�
						///���x������Ұ�Ƕ�,ˮƽFOVȡ�����ӿڵĿ�߱�

	//�������
	glm::vec3 _position;	///���λ��(world space)
	glm::vec3 _front;		///�������(world space),�������ָ��
	glm::vec3 _right;		///����Ҳ�(world space),��������ռ��x��������
	glm::vec3 _up;			///����Ϸ�(world space),

	/// �����������
	void updateCameraAttr();

	void UpdateEular();
};