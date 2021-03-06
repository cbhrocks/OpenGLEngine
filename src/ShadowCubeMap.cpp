#include "ShadowCubeMap.h"
#include "glHelper.h"

ShadowCubeMap::ShadowCubeMap(const glm::vec3 position, GLsizei resX, GLsizei resY, GLfloat shadowNear, GLfloat shadowFar) :
	position(position), shadowResX(resX), shadowResY(resY), shadowNear(shadowNear), shadowFar(shadowFar)
{
	glGenFramebuffers(1, &this->shadowBuffer);
	checkGLError("BasicLight::genShadowMap");

	glGenTextures(1, &this->texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->texture);
	for (GLuint i = 0; i < 6; ++i) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, resX, resY, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	}
	checkGLError("BasicLight::genShadowMap");

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	checkGLError("BasicLight::genShadowMap");

	glBindFramebuffer(GL_FRAMEBUFFER, this->shadowBuffer);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, this->texture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	checkGLError("BasicLight::genShadowMap");

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	checkGLError("BasicLight::genShadowMap");
}

void ShadowCubeMap::setActive() {
	glViewport(0, 0, this->shadowResX, this->shadowResY);
	glBindFramebuffer(GL_FRAMEBUFFER, this->shadowBuffer);
	glClear(GL_DEPTH_BUFFER_BIT);
}

void ShadowCubeMap::uploadUniforms(const Shader& shader) {
	shader.Use();
	std::vector<glm::mat4> transforms = this->getShadowTransforms();
	for (int i = 0; i < transforms.size(); i++) {
		shader.setMat4("shadowTransforms[" + std::to_string(i) + "]", transforms.at(i));
	}
	shader.setFloat("shadowFar", this->shadowFar);
	shader.setVec3("lightPos", this->position);
	checkGLError("BasicLight::drawShadowMap -- upload matrices");
}

std::vector<glm::mat4> ShadowCubeMap::getShadowTransforms() {
	float aspect = (float)this->shadowResX / (float)this->shadowResY;
	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, this->shadowNear, this->shadowFar);

	std::vector<glm::mat4> shadowTransforms;
	shadowTransforms.push_back(shadowProj * glm::lookAt(this->position, this->position + glm::vec3( 1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(this->position, this->position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(this->position, this->position + glm::vec3( 0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(this->position, this->position + glm::vec3( 0.0,-1.0, 0.0), glm::vec3(0.0, 0.0,-1.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(this->position, this->position + glm::vec3( 0.0, 0.0, 1.0), glm::vec3(0.0,-1.0, 0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(this->position, this->position + glm::vec3( 0.0, 0.0,-1.0), glm::vec3(0.0,-1.0, 0.0)));
	return shadowTransforms;
}
