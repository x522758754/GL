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
#endif