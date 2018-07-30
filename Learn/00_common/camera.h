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

	/// 当前俯仰角
	float get_pitch();

	/// 当前偏航角
	float get_yaw();

	/// 当前视野角度
	float get_fovy();

	/// 相机当前位置
	glm::vec3 get_position();

	/// 相机当前朝向
	glm::vec3 get_front();

	/// 当前视矩阵
	glm::mat4 GetViewMatrix();

	/// 测试固定欧拉角偏转
	void handleEular(Camera_Eular direction);

	/// 按键处理
	void handleKey(Camera_Movement direction, float deltaTime);

	/// 鼠标处理
	void handleMouse(float x, float y);

	/// 滚轴处理
	void handleWheel(float y);

	void set_position(glm::vec3 pos);

private:
	///不考虑使用滚转角(roll),在此使用固定的上向量
	///const glm::vec3 _worldup = glm::vec3(0, 1, 0); 

	//欧拉角(Euler Angle) 右手坐标系
	///避免万向锁,控制pitch在[-89.0,89.0]的范围内，控制yaw在[0,360.0]范围内
	float _pitch;	///俯仰角(单位:°) 头绕x轴
	float _yaw;		///偏航角(单位:°) 头绕y轴
	float _roll;	///翻滚角(单位:°) 头绕z轴;此处不考虑

	//相机控制
	float _zoomSpeed;	///相机推拉速度
	float _rotateSpeed;	///相机转向速度(pitchSpeed、yawSpeed都用此速度)
	float _fovy;		///相机y方向视野角度,视锥体的垂直方向上的角度
						///相机x方向视野角度,水平FOV取决于视口的宽高比

	//相机属性
	glm::vec3 _position;	///相机位置(world space)
	glm::vec3 _front;		///相机方向(world space),代表相机指向
	glm::vec3 _right;		///相机右侧(world space),代表相机空间的x轴正方向
	glm::vec3 _up;			///相机上方(world space),

	/// 更新相机属性
	void updateCameraAttr();

	void UpdateEular();
};