#pragma once

#include <iostream>
#include "vertexData.h"
#include "shader.h"
#include "glHelper.h"

class FBOManagerI {
public:
	GLuint textures[1];
	GLuint FBO;
	GLuint RBO;
	GLuint VAO;
	GLuint VBO;
	size_t width;
	size_t height;
	Shader shader;

	virtual void setup() = 0;

	virtual void setShader(Shader shader) = 0;
	virtual void setDimensions(size_t width, size_t height) = 0;
	virtual void createTextures() = 0;
	virtual void createRBO() = 0;
	virtual void createVAO() = 0;
	virtual void setActive() = 0;
	virtual void UploadUniforms() = 0;
	virtual void UploadUniforms(Shader shader) = 0;
	virtual void Draw() = 0;
	virtual void Draw(Shader shader) = 0;
};

class FBOManager : public FBOManagerI{
public:
	GLuint textures[1];
	GLuint FBO;
	GLuint RBO;
	GLuint VAO;
	GLuint VBO;
	size_t width;
	size_t height;
	Shader shader;

	FBOManager();
	FBOManager(size_t width, size_t height, Shader shader);

	void setup();

	virtual void setShader(Shader shader);
	virtual void setDimensions(size_t width, size_t height);
	virtual void createTextures();
	virtual void createRBO();
	virtual void createVAO();
	virtual void setActive();
	virtual void Draw()
	{ this->Draw(this->shader); }
	virtual void UploadUniforms()
	{ this->UploadUniforms(this->shader); }
	virtual void UploadUniforms(Shader shader);
	virtual void Draw(Shader shader);
};

class HDRBuffer : public FBOManager{
public:
	HDRBuffer();
	HDRBuffer(size_t width, size_t height, Shader shader);

	void createTextures();
};


class BloomBuffer : public HDRBuffer{
public:
	Shader blurShader;
	GLuint textures[2];
	GLuint pingpongFBOs[2];
	GLuint pingpongTextures[2];

	BloomBuffer();
	BloomBuffer(size_t width, size_t height, Shader shader, Shader blurShader);

	void setup();

	void createTextures();
	void createRBO();
	using FBOManager::Draw;
	void Draw(Shader shader);
	using FBOManager::UploadUniforms;
	void BloomBuffer::UploadUniforms(Shader shader);
};
