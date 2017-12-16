#include "Utils.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int Utils::loadTexture(char const * path, bool gmmaCorrection/* = false*/)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum internalFormat, dataFormat;
		switch (nrComponents)
		{
		case 1:
			internalFormat = dataFormat = GL_RED;
			break;
		case 3:
			internalFormat = gmmaCorrection ? GL_SRGB : GL_RGB;
			dataFormat = GL_RGB;
			break;
		case 4:
			internalFormat = gmmaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
			dataFormat = GL_RGBA;
			break;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

// utility function for loading cube texture from file
// ---------------------------------------------------
unsigned int Utils::loadCubemap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i != faces.size(); ++i)
	{
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);

		if (data)
		{
			GLenum format;
			if (nrChannels == 1)
				format = GL_RED;
			else if (nrChannels == 3)
				format = GL_RGB;
			else if (nrChannels == 4)
				format = GL_RGBA;
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i].c_str() << std::endl;
		}

		stbi_image_free(data);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	return textureID;
}

int Utils::Calc()
{
	return 0;
}

//calculate fps
char* Utils::CalculateFrameRate()
{
	char szFPS[2048];
	static float framesPerSecond = 0.0f;     //帧计数器
	static float lastTime = 0.0f;

	//将毫秒转换成秒
	float currentTime = GetTickCount() * 0.001f;

	// 累加帧计数器
	++framesPerSecond;

	// 如果当前时间和上次记录的时间差大于1秒
	if (currentTime - lastTime > 1.0f)
	{
		//将上一次的记录时间设成当前时间
		lastTime = currentTime;

		//记入szFPS全局变量
		sprintf_s(szFPS, "Current Frames Per Second: %d", int(framesPerSecond));

		std::cout << framesPerSecond << std::endl;

		// 对帧计数器进行复位
		framesPerSecond = 0;
	}

	return szFPS;
}