#pragma once

#include <iostream>
#include "vertexData.h"
#include "shader.h"
#include "glHelper.h"
#include "Icosphere.h"
#include "light.h"

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
	void UploadUniforms(const Shader& shader);
};

class GBuffer {
public:
	GBuffer(GLsizei width, GLsizei height);

	GLuint getPositionTexture() { return this->gPosition; }
	GLuint getNormalTexture() { return this->gNormal; }
	GLuint gAlbedoSpecTexture() { return this->gAlbedoSpec; }

	///<summary>set the gBuffer active so that everything drawn will be drawn into it</summary>
	void setActive();

	void BeginLightPasses();

	///<summary>using a quad set to teh size of the entire screen, all data stored in the geometry buffers is rendered.
	///</summary>
	void DSDirectionLightPass(const Shader& shader);

	///<summary>renders a sphere for each point light with radius set to the effective lighting range of the light. This will prevent unecessary lighting calculations from being applied to geometry in the scene. 
	///</summary>
	///<param name="shader">The deferred shading point light shader. should have at least three inputs: gPosition, gNormal, gAlbedoSpec</param>
	///<param name="plgiths">A deferred shading point light shader.</param>
	void DSPointLightPass(const Shader& shader, std::vector<PointLight*> plights);

	///<summary>copies the gBuffer depth data into the specified frame buffer object.
	///<para>This is useful for combining forward rendering with deferred rendering, as you can get proper visual occlusion on objects that are forward rendered.</para>
	///</summary>
	///<param name="fbo">The target frame buffer to copy depth into.</param>
	///<param name="width">The width of the the target frame buffer</param>
	///<param name="height">The height of the the target frame buffer</param>
	void copyDepth(GLuint fbo, GLsizei width, GLsizei height);

private:
	// textures used by gBuffer. gPosition: world space position, gNormal: world space surface normal, gAlbedoSpec: albedo color with specular intensity alpha channel
	GLuint gPosition, gNormal, gAlbedoSpec; 
	GLuint gBuffer; // fbo
	GLuint depthBuffer; // rbo
	GLuint quadVBO, quadVAO = 0; // vao for drawing 2d scene
	GLsizei width, height;

	///<summary>the sphere used to set the boudries for point light shading</summary>
	Model* pLightSphere;

	/*
	if the quad VAO has not been created, create it.
	draw the quad.
	*/
	void drawQuad();
};
