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

/// 创建一个颜色帧缓冲
int genfboColor(GLuint index, GLuint* texColorBuffer);

/// 创建一个深度帧缓冲
int genfboDepth(GLuint* depthMap, int mapW, int mapH);

/// 创建一个深度帧缓冲，立方体贴图纹理
int genfboDepthCube(GLuint* texture, int mapW, int mapH);

/// 创建一个模板帧缓冲
int genfboStencil();

/// 创建一个深度模板帧缓冲
int genfboDepthStencil();

/// 创建一个多采样帧缓冲
int genfboMS(int samples, GLuint* textureMS);

/// 创建一个浮点帧缓冲
int genfboFloatingPoint(GLuint &tex, int mapW, int mapH);

/// 创建一个浮点帧缓冲多个color buffers

int genfboFloatingPointMultiColor(GLuint* tex, int mapW, int mapH, int texCount);

/// 创建一个深度模板渲染对象附件
int genrbo();

/// 绘制一个立方体
void drawCube(GLuint* VAO);

/// 绘制一个平面
void drawPlane(GLuint* VAO);

/// 绘制一个标准化设备坐标下的全屏的面片
void drawQuad(GLuint* VAO);

/// 绘制天空盒
void drawSkybox(GLuint* VAO);

/// 获取
glm::vec3 getCameraUp(glm::vec3 cameraPos, glm::vec3 targetPos);
#endif