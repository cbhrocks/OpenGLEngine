#pragma once

#include <iostream>
#include "vertexData.h"
#include "shader.h"
#include "glHelper.h"
#include "DrawObj.h"

class FBOManagerI {
public:
	virtual void setup() = 0;

	virtual void setShader(const Shader* shader) = 0;
	virtual void setDimensions(GLsizei width, GLsizei height) = 0;
	virtual void createVAO() = 0;
	virtual void setActive() = 0;
	virtual void UploadUniforms() = 0;
	virtual void UploadUniforms(const Shader& shader) = 0;
	virtual void Draw() = 0;
	virtual void Draw(const Shader& shader) = 0;
};

class FBOManager : public FBOManagerI{
public:
	GLuint textures[1];
	GLuint FBO;
	GLuint RBO;
	GLuint VAO;
	GLuint VBO;
	GLsizei width;
	GLsizei height;
	const Shader* shader;

	FBOManager();
	FBOManager(GLsizei width, GLsizei height, Shader* shader);

	void setup();

	void setShader(const Shader* shader);
	virtual void setDimensions(GLsizei width, GLsizei height);
	void createVAO();
	virtual void setActive();
	void Draw()
	{ this->Draw(*this->shader); }
	void UploadUniforms()
	{ this->UploadUniforms(*this->shader); }
	virtual void UploadUniforms(const Shader& shader);
	virtual void Draw(const Shader& shader);
};

class HDRBuffer : public FBOManager{
public:
	HDRBuffer(GLsizei width, GLsizei height, Shader* shader);

	void setup();
};


class BloomBuffer : public FBOManager{
public:
	Shader* blurShader;
	GLuint textures[2];
	GLuint pingpongFBOs[2];
	GLuint pingpongTextures[2];

	BloomBuffer(GLsizei width, GLsizei height, Shader* shader, Shader* blurShader);

	void setup();

	using FBOManager::setShader;
	using FBOManager::Draw;
	void Draw(const Shader& shader);
	using FBOManager::UploadUniforms;
	void BloomBuffer::UploadUniforms(const Shader& shader);
};

class GBuffer {
public:
	GLuint gPosition, gNormal, gAlbedoSpec; // world space position, surface normal, albedo base color 
	GLuint gBuffer; // fbo
	GLuint depthBuffer; // rbo
	GLuint VAO;
	GLuint VBO;
	GLsizei width;
	GLsizei height;
	Shader* shader;

	GBuffer();
	GBuffer(GLsizei width, GLsizei height, Shader* shader);

	void setup();

	virtual void setShader(Shader* shader);

	virtual void DrawToBuffer(std::vector<DrawObject*> objs)
	{ this->DrawToBuffer(this->shader, objs); }
	virtual void DrawToBuffer(const Shader* shader, std::vector<DrawObject*> objs);

	//virtual void UploadUniforms()
	//{ this->UploadUniforms(this->shader); }
	//virtual void UploadUniforms(Shader shader);
};
