#include "ShadowMap.h"
#include "glHelper.h"

ShadowMap::ShadowMap(DirectionLight& light, GLsizei resX, GLsizei resY, GLsizei shadowWidth, GLsizei shadowHeight, GLsizei shadowNear, GLsizei shadowFar, glm::vec3 position) :
	light(light), shadowWidth(shadowWidth), shadowHeight(shadowHeight), shadowNear(shadowNear), shadowFar(shadowFar), position(position)
{
	glGenFramebuffers(1, &this->shadowBuffer);
	checkGLError("DirectionLight::genShadowMap");

	glGenTextures(1, &this->texture);
	glBindTexture(GL_TEXTURE_2D, this->texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, resX, resY, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	checkGLError("DirectionLight::genShadowMap");

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	checkGLError("DirectionLight::genShadowMap");

	glBindFramebuffer(GL_FRAMEBUFFER, this->shadowBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->texture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	checkGLError("DirectionLight::genShadowMap");

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	checkGLError("DirectionLight::genShadowMap");
}

void ShadowMap::setActive() {
	glViewport(0, 0, this->shadowWidth, this->shadowHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, this->shadowBuffer);
	glClear(GL_DEPTH_BUFFER_BIT);
}

void ShadowMap::uploadUniforms(const Shader& shader) {
	shader.Use();
	glm::mat4 transform = this->getShadowTransform();
	shader.setMat4("shadowTransform", transform);
	shader.setFloat("far", this->shadowFar);
	shader.setVec3("pos", this->position);
	checkGLError("BasicLight::drawShadowMap -- upload matrices");
}

glm::mat4 ShadowMap::getShadowTransform() {
	glm::mat4 shadowProjection = glm::ortho(-this->shadowWidth/2, this->shadowWidth/2, -this->shadowHeight/2, this->shadowHeight/2, this->shadowNear, this->shadowFar);
	glm::mat4 shadowView = glm::lookAt(this->position, this->position + this->light.getDirection(), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 shadowTransform = shadowProjection * shadowView;
	return shadowTransform;
}
