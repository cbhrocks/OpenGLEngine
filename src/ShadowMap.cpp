#include "ShadowMap.h"

ShadowMap::ShadowMap(const glm::vec3 position, const glm::vec3 direction, GLsizei resX, GLsizei resY, GLsizei shadowWidth, GLsizei shadowHeight, float shadowNear, float shadowFar) :
	position(position), direction(direction), shadowResX(resX), shadowResY(resY), shadowWidth(shadowWidth), shadowHeight(shadowHeight), shadowNear(shadowNear), shadowFar(shadowFar)
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
	glViewport(0, 0, this->shadowResX, this->shadowResY);
	glBindFramebuffer(GL_FRAMEBUFFER, this->shadowBuffer);
	glClear(GL_DEPTH_BUFFER_BIT);
}

void ShadowMap::uploadUniforms(const Shader& shader) {
	shader.Use();
	glm::mat4 transform = this->getShadowTransform();
	shader.setMat4("shadowTransform", transform);
	checkGLError("ShadowMap::drawShadowMap -- upload matrices");
}

glm::mat4 ShadowMap::getShadowTransform() {
	glm::mat4 shadowProjection = glm::ortho(
		(float) - this->shadowWidth/2, 
		(float) this->shadowWidth/2, 
		(float) - this->shadowHeight/2, 
		(float) this->shadowHeight/2, 
		this->shadowNear, 
		this->shadowFar
	);
	glm::mat4 shadowView = glm::lookAt(
		this->position, 
		this->position + this->direction, 
		glm::vec3(0.0f, 1.0f, 0.0f)
	);
	glm::mat4 shadowTransform = shadowProjection * shadowView;
	return shadowTransform;
}

void ShadowMap::drawDebugCube(const Shader& shader) {
	if (this->debugCubeVAO == 0)
    {
		glGenVertexArrays(1, &this->debugCubeVAO);
		glGenBuffers(1, &this->debugCubeVBO);
		glBindBuffer(GL_ARRAY_BUFFER, this->debugCubeVBO);
		glBufferData(GL_ARRAY_BUFFER, cubePositions.size() * sizeof(glm::vec3), &cubePositions.front(), GL_STATIC_DRAW);
		glBindVertexArray(this->debugCubeVAO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
		glEnableVertexAttribArray(0);
		glBindVertexArray(0);
    }
	glm::mat4 ModelMat = glm::mat4(1.0f);
	ModelMat = glm::scale(glm::translate(ModelMat, this->position), glm::vec3(0.25, 0.25, 0.25));
	shader.setMat4("Model", ModelMat);

	glBindVertexArray(this->debugCubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	shader.Use();
}

void ShadowMap::drawDebugQuad(const Shader& shader) {
	if (this->debugVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &this->debugVAO);
        glGenBuffers(1, &this->debugVBO);
        glBindVertexArray(this->debugVAO);
        glBindBuffer(GL_ARRAY_BUFFER, this->debugVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		//cleanup
		//glDeleteBuffers(1, &this->debugVBO);
    }
	shader.Use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->texture);

    glBindVertexArray(this->debugVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
	checkGLError("ShadowMap::drawDebugQuad -- end");
}
