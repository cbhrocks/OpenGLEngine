#pragma once
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader.h"
//#include <glm/gtc/type_ptr.hpp>

class ShadowCubeMap {
public:

	ShadowCubeMap(glm::vec3 position, GLsizei resX, GLsizei resY, GLfloat shadowNear, GLfloat shadowFar);

	///<summary>Set the viewport dimensions to that of the shadowMap and bind the shadowbuffer
	///<para>This should be called before the shadow pass.</para>
	///</summary>
	void setActive();

	///<summary>Get the texture that was created by the shadow pass.</summary>
	GLuint getTexture() { return this->texture; }

	///<summary>Upload uniforms needed during the shadow pass.</summary>
	void uploadUniforms(const Shader& shader);

	const glm::vec3 getPosition() const { return this->position; }
	void setPosition(glm::vec3 position) { this->position = position; }

private:
	GLuint texture;
	GLuint shadowBuffer;
	GLsizei shadowResX, shadowResY;
	GLfloat shadowNear, shadowFar;
	glm::vec3 position;

	///<summary>return a vector of all the transforms that need to be applied to the shadowmap based on its light's position.</summary>
	std::vector<glm::mat4> getShadowTransforms();
};
