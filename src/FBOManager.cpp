#pragma once

#include "FBOManager.h"

FBOManager::FBOManager() { }

FBOManager::FBOManager(size_t width, size_t height, Shader* shader) :
	width(width), height(height), shader(shader)
{
	this->setup();
}

void FBOManager::setup() 
{
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	this->createTextures();
	this->createRBO();

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	checkGLError("Initial FBOManager");
}

void FBOManager::setShader(Shader* shader)
{
	this->shader = shader;
}

void FBOManager::setDimensions(size_t width, size_t height)
{
	this->width = width;
	this->height = height;
	this->createTextures();
}

void FBOManager::createTextures()
{
	glGenTextures(1, this->textures);
	glBindTexture(GL_TEXTURE_2D, this->textures[0]);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->width, this->height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->textures[0], 0);

	glBindTexture(GL_TEXTURE_2D, 0);

	checkGLError("create texture FBOManager");
}

void FBOManager::createRBO()
{
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, this->width, this->height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	checkGLError("create rbo FBOManager");
}

void FBOManager::createVAO()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertexData), quadVertexData, GL_STATIC_DRAW);

	glBindVertexArray(VAO);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//texture coords
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glBindVertexArray(0);

	checkGLError("create vao FBOManager");
}

void FBOManager::setActive()
{
	glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);

	glStencilMask(0xFF);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void FBOManager::UploadUniforms(Shader* shader)
{
	shader->setInt("texId", 0);
	checkGLError("FBOManager::UploadUniforms");
}

void FBOManager::Draw(Shader* shader)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	checkGLError("FBOManager::Draw -- prep opengl");

	shader->Use();
	this->UploadUniforms(shader);
	checkGLError("FBOManager::Draw -- upload uniforms");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->textures[0]);
	checkGLError("FBOManager::Draw -- bind texture");

	glBindVertexArray(this->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	checkGLError("FBOManager::Draw -- draw");

	glEnable(GL_DEPTH_TEST);

	checkGLError("FBOManager::Draw");
}

HDRBuffer::HDRBuffer()
{
}

HDRBuffer::HDRBuffer(size_t width, size_t height, Shader* shader) :
	FBOManager(width, height, shader)
{
}

void HDRBuffer::createTextures()
{
	glGenTextures(1, this->textures);
	glBindTexture(GL_TEXTURE_2D, this->textures[0]);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, this->width, this->height, 0, GL_RGBA, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->textures[0], 0);

	glBindTexture(GL_TEXTURE_2D, 0);

	checkGLError("create texture FBOManager");
}

BloomBuffer::BloomBuffer()
{
}

BloomBuffer::BloomBuffer(size_t width, size_t height, Shader* shader, Shader* blurShader) :
	HDRBuffer(width, height, shader), blurShader(blurShader)
{
}

void BloomBuffer::createTextures()
{
	glGenTextures(2, this->textures);
	for (int i = 0; i < 2; i++) {
		glBindTexture(GL_TEXTURE_2D, this->textures[i]);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, this->width, this->height, 0, GL_RGBA, GL_FLOAT, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, this->textures[i], 0);
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	checkGLError("BloomBuffer::createTextures");
}

void BloomBuffer::createRBO()
{
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, this->width, this->height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
	GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	checkGLError("BloomBuffer::createRBO");
}

void BloomBuffer::UploadUniforms(Shader* shader)
{
	shader->setInt("sceneTex", 0);
	shader->setInt("bloomTex", 1);
	checkGLError("FBOManager::UploadUniforms");
}

void BloomBuffer::Draw(Shader* shader)
{
	bool horizontal = true, first_iteration = true;
	int amount = 10;
	blurShader->Use();
	blurShader->setInt("image", 0);
	for (unsigned int i = 0; i < amount; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBOs[horizontal ? 1 : 0]);
		blurShader->setInt("horizontal", horizontal);
		glBindTexture(GL_TEXTURE_2D, first_iteration ? this->textures[1] : this->pingpongTextures[horizontal ? 0 : 1]);

		glBindVertexArray(this->VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		horizontal = !horizontal;
		if (first_iteration)
			first_iteration = false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	checkGLError("BloomBuffer::Draw -- prep opengl");

	shader->Use();
	this->UploadUniforms(shader);
	checkGLError("BloomBuffer::Draw -- upload uniforms");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->textures[0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, this->pingpongTextures[horizontal ? 0 : 1]);
	checkGLError("BloomBuffer::Draw -- bind textures");

	glBindVertexArray(this->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	checkGLError("BloomBuffer::Draw -- draw");

	glEnable(GL_DEPTH_TEST);

	checkGLError("BloomBuffer::Draw");
}

void BloomBuffer::setup() 
{
	HDRBuffer::setup();

	glGenFramebuffers(2, this->pingpongFBOs);
	glGenTextures(2, this->pingpongTextures);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, this->pingpongFBOs[i]);
		glBindTexture(GL_TEXTURE_2D, this->pingpongTextures[i]);
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGB16F, this->width, this->height, 0, GL_RGB, GL_FLOAT, NULL
		);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->pingpongTextures[i], 0);
	}
}

GBuffer::GBuffer()
{
}

GBuffer::GBuffer(size_t width, size_t height, Shader* shader) :
	width(width), height(height), shader(shader)
{
}

void GBuffer::setShader(Shader* shader) {
	this->shader = shader;
}

void GBuffer::setDimensions(size_t width, size_t height) {
	this->width = width;
	this->height = height;
}

void GBuffer::createVAO() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertexData), quadVertexData, GL_STATIC_DRAW);

	glBindVertexArray(VAO);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//texture coords
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glBindVertexArray(0);

	checkGLError("create vao FBOManager");
}

void GBuffer::setup()
{
	glGenFramebuffers(1, &this->gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, this->gBuffer);

	// position color buffer
	glGenTextures(1, &this->gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, this->width, this->height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	// normal color buffer
	glGenTextures(1, &this->gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, this->width, this->height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

	// albedo + spec intensity color buffer
	glGenTextures(1, &this->gAlbedoSpec);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, this->width, this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

	GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);

	glGenRenderbuffers(1, &this->depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, this->depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, this->width, this->height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->depthBuffer);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GBuffer::DrawToBuffer(Shader* shader, std::vector<DrawObject*> objs)
{
	glBindFramebuffer(GL_FRAMEBUFFER, this->gBuffer);

	shader->Use();

	shader->setInt("gPosition", 0);
	shader->setInt("gNormal", 1);
	shader->setInt("gAlbedoSpec", 2);

	for (int i = 0; i < objs.size(); i++) {
		objs.at(i)->UploadUniforms(shader);
		objs.at(i)->Draw(shader);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
