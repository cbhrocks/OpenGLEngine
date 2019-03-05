#pragma once
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include <glad/glad.h> // holds all OpenGL type declarations

#include "stb_image.h"

enum TextureType {
	mesh,
	plane,
};

class TextureLoader
{
public:
	std::map<std::string, GLuint> texIds;

	TextureLoader()
	{}

	unsigned int TextureFromFile(const char *path, const std::string &directory, TextureType type)
	{

		std::string filename = std::string(path);
		filename = directory + '/' + filename;

		std::cout << "filename: " << filename << std::endl;

		if (this->texIds.count(filename))
			return this->texIds[filename];

		GLuint textureID;
		glGenTextures(1, &textureID);

		int width, height, nrComponents;
		unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
		if (data)
		{
			GLenum format;
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;

			createTexture(textureID, format, width, height, data, type);

			stbi_image_free(data);
		}
		else
		{
			std::cout << "Texture failed to load at path: " << path << std::endl;
			stbi_image_free(data);
		}

		this->texIds[filename] = textureID;

		return textureID;
	}

private:
	void createTexture(GLuint textureID, GLenum format, int width, int height, unsigned char *data, TextureType)
	{
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
};
