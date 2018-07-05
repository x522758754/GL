#pragma once
#ifndef UTILS_H
#define UTILS_H

#include <glad/glad.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>

class Utils
{
public:
	

	static unsigned int loadTexture(char const * path, bool gmmaCorrection = false);
	static unsigned int loadCubemap(std::vector<std::string> faces);
	static unsigned int loadHDRImgae(char const * path);
	static int Calc();
	static char* CalculateFrameRate();
	static void renderCube();
	static void renderSphere();

private:
	static unsigned int indexCount, sphereVAO; // sphere
	static unsigned int cubeVAO;				// cube
};

/// ����һ����ɫ֡����
int genfboColor(GLuint index, GLuint* texColorBuffer);

/// ����һ�����֡����
int genfboDepth(GLuint* depthMap, int mapW, int mapH);

/// ����һ�����֡���壬��������ͼ����
int genfboDepthCube(GLuint* texture, int mapW, int mapH);

/// ����һ��ģ��֡����
int genfboStencil();

/// ����һ�����ģ��֡����
int genfboDepthStencil();

/// ����һ�������֡����
int genfboMS(int samples, GLuint* textureMS);

/// ����һ������֡����
int genfboFloatingPoint(GLuint &tex, int mapW, int mapH);

/// ����һ������֡������color buffers

int genfboFloatingPointMultiColor(GLuint* tex, int mapW, int mapH, int texCount);

/// ����һ�����ģ����Ⱦ���󸽼�
int genrbo();

/// ����һ��������
void drawCube(GLuint* VAO);

/// ����һ��ƽ��
void drawPlane(GLuint* VAO);

/// ����һ����׼���豸�����µ�ȫ������Ƭ
void drawQuad(GLuint* VAO);

/// ������պ�
void drawSkybox(GLuint* VAO);

/// ��ȡ
glm::vec3 getCameraUp(glm::vec3 cameraPos, glm::vec3 targetPos);
#endif