#pragma once

#include "FBOManager.h"

FBOManager::FBOManager() { }

FBOManager::FBOManager(GLsizei width, GLsizei height, Shader* shader) :
	width(width), height(height), shader(shader)
{
}

void FBOManager::setup() 
{
	glGenFramebuffers(1, &this->FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);

	// create the texture that will be used to paint the quad
	glGenTextures(1, this->textures);
	glBindTexture(GL_TEXTURE_2D, this->textures[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->width, this->height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->textures[0], 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	checkGLError("FBOManager::setup -- create texture");

	// create the renderbuffer that includes depth and stencil
	glGenRenderbuffers(1, &this->RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, this->RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, this->width, this->height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	checkGLError("FBOManager::setup -- create rbo");

	// check to make sure complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	checkGLError("Initial FBOManager");

	this->createVAO();
}

void FBOManager::setShader(const Shader* shader)
{
	this->shader = shader;
}

void FBOManager::setDimensions(GLsizei width, GLsizei height)
{
	this->width = width;
	this->height = height;
	this->setup();
}

/*
This method is called in order to create a quad that the rendered image is going to be placed on.
*/
void FBOManager::createVAO()
{
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertexData), quadVertexData, GL_STATIC_DRAW);

	glBindVertexArray(this->VAO);

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

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void FBOManager::UploadUniforms(const Shader& shader)
{
	shader.setInt("texId", 0);
	checkGLError("FBOManager::UploadUniforms");
}

void FBOManager::Draw(const Shader& shader)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	checkGLError("FBOManager::Draw -- prep opengl");

	shader.Use();
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

HDRBuffer::HDRBuffer(GLsizei width, GLsizei height, Shader* shader) :
	FBOManager(width, height, shader)
{
}

void HDRBuffer::setup()
{
	glGenFramebuffers(1, &this->FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);

	// create the texture that will be used to paint the quad
	glGenTextures(1, this->textures);
	glBindTexture(GL_TEXTURE_2D, this->textures[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, this->width, this->height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->textures[0], 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	checkGLError("HDRBuffer::setup -- create textures");

	// create the renderbuffer that includes depth and stencil
	glGenRenderbuffers(1, &this->RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, this->RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, this->width, this->height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	checkGLError("FBOManager::setup -- create rbo");

	// check to make sure complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	checkGLError("Initial FBOManager");

	this->createVAO();
}

BloomBuffer::BloomBuffer(GLsizei width, GLsizei height, Shader* shader, Shader* blurShader) :
	FBOManager(width, height, shader), blurShader(blurShader)
{
}

void BloomBuffer::setup() 
{
	//HDRBuffer::setup();
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	// create the texture that will be used to paint the quad
	glGenTextures(2, this->textures);
	for (int i = 0; i < 2; i++) {
		glBindTexture(GL_TEXTURE_2D, this->textures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, this->width, this->height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, this->textures[i], 0);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	checkGLError("BloomBuffer::setup -- create textures");

	// create the renderbuffer that includes depth and stencil, has two attachments for two colors.
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, this->width, this->height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
	GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	checkGLError("BloomBuffer::setup -- create renderbuffer");

	// check to make sure complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	checkGLError("Initial FBOManager");

	this->createVAO();

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

void BloomBuffer::UploadUniforms(const Shader& shader)
{
	shader.setInt("sceneTex", 0);
	shader.setInt("bloomTex", 1);
	checkGLError("FBOManager::UploadUniforms");
}

void BloomBuffer::Draw(const Shader& shader)
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

	shader.Use();
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

GBuffer::GBuffer(int width, int height): width(width), height(height), pLightSphere(new Model("lightVolume", std::make_unique<Sphere>("lightVolume")))
{
	this->initialize(width, height);
}

void GBuffer::initialize(int width, int height) {
	glGenFramebuffers(1, &this->gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, this->gBuffer);

	// position color buffer
	glGenTextures(1, &this->gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	// normal color buffer
	glGenTextures(1, &this->gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

	// albedo + spec intensity color buffer
	glGenTextures(1, &this->gAlbedoSpec);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

	GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);

	glGenRenderbuffers(1, &this->depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, this->depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_STENCIL, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->depthBuffer);

	// final color texture.
	glGenTextures(1, &this->gFinal);
	glBindTexture(GL_TEXTURE_2D, this->gFinal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, this->gFinal, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GBuffer::setDimensions(int width, int height) {
	this->width = width;
	this->height = height;
	this->initialize(width, height);
}

void GBuffer::BindForWriting()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->gBuffer);

	std::vector<GLenum> drawBuffers = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, drawBuffers.data());

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GBuffer::DSLightingPass(const Shader& directionShader, const Shader& pointShader, const Shader& depthShader, std::vector<PointLight*> plights) {
	glBindFramebuffer(GL_FRAMEBUFFER, this->gBuffer);
	glDrawBuffer(GL_COLOR_ATTACHMENT3);
	glClear(GL_COLOR_BUFFER_BIT);

	// dont want to write to any color buffers.
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_STENCIL_TEST);
	// stencil pass depends on depth buffer, but should not write to it
	glDepthMask(GL_FALSE);


	for (PointLight* plight : plights) {
		///////////////////////////////////////////////////////////////////////////////////////////
		// stencil pass
		///////////////////////////////////////////////////////////////////////////////////////////

		depthShader.Use();

		// dont want to actualy draw at all
		glDrawBuffer(GL_NONE);
		// need stencil to trigger on both faces
		glDisable(GL_CULL_FACE);

		glClear(GL_STENCIL_BUFFER_BIT);
		// want stencil func to always be succeed. only using depth here
		glStencilFunc(GL_ALWAYS, 0, 0);

		glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
		glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

		this->pLightSphere->setScale(glm::vec3(plight->getRadius()));
		this->pLightSphere->setPosition(plight->getPosition());
		pLightSphere->uploadUniforms(depthShader);
		this->pLightSphere->Draw(depthShader);

		//disable depth testing and depth mask so the lighting pass cannot write to the depth buffer
		glEnable(GL_CULL_FACE);


		///////////////////////////////////////////////////////////////////////////////////////////
		// point lighting pass
		///////////////////////////////////////////////////////////////////////////////////////////
		glDrawBuffer(GL_COLOR_ATTACHMENT3);

		// dont need depth
		glDisable(GL_DEPTH_TEST);
		// enable blending so that we can handle directional and point lighting in different passes.
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);

		// cull the back face to allow lighting when view position is in sphere
		glCullFace(GL_FRONT);

		glStencilFunc(GL_NOTEQUAL, 0, 0xFF);

		pointShader.Use();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, this->gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, this->gAlbedoSpec);

		//glClear(GL_COLOR_BUFFER_BIT);

		// for each light move the sphere to it's location and set radius its max attenuation distance. then draw the sphere.
		plight->uploadUniforms(pointShader);
		this->pLightSphere->setScale(glm::vec3(plight->getRadius()));
		this->pLightSphere->setPosition(plight->getPosition());
		pLightSphere->uploadUniforms(pointShader);

		this->pLightSphere->Draw(pointShader);

		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		glCullFace(GL_BACK);
	}

	glDisable(GL_STENCIL_TEST);

	///////////////////////////////////////////////////////////////////////////////////////////
	// direction lighting pass
	///////////////////////////////////////////////////////////////////////////////////////////
	glDrawBuffer(GL_COLOR_ATTACHMENT3);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);

	directionShader.Use();

	this->drawQuad();

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	glDepthMask(GL_TRUE);
}

void GBuffer::copyDepth(GLuint fbo, GLsizei targetWidth, GLsizei targetHeight) {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, this->gBuffer);
	glReadBuffer(GL_COLOR_ATTACHMENT3);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
	glBlitFramebuffer(0, 0, this->width, this->height, 0, 0, targetWidth, targetHeight, GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	checkGLError("GBuffer::copyDepth");
}

void GBuffer::drawQuad() {
	if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		//cleanup
		glBindVertexArray(0);
		glDeleteBuffers(1, &quadVBO);
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}
