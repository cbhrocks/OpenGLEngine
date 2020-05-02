#pragma once
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "light.h"
//#include <glm/gtc/type_ptr.hpp>

class ShadowMap {
public:
	///<summary>construct FBO for shadow map and set private member variables.
	///</summary>
	///<param name="resX">The x resolution of the shadow texture map.</param>
	///<param name="resY">The y resolution of the shadow texture map.</param>
	///<param name="light">The DirectionLight associated with this shadow map.</param>
	///<param name="shadowWidth">The width of the orthographic projection matrix used to generate the shadow texture map.</param>
	///<param name="shadowHeight">The height of the orthographic projection matrix used to generate the shadow texture map.</param>
	///<param name="shadowNear">The near bound of the orthographic projection matrix used to generate the shadow texture map.</param>
	///<param name="shadowFar">The far bound of the orthographic projection matrix used to generate the shadow texture map.</param>
	///<param name="position">The position of the orthographic projection matrix used to generate the shadow texture map.</param>
	ShadowMap(DirectionLight& light, GLsizei resX, GLsizei resY, GLsizei shadowWidth, GLsizei shadowHeight, GLsizei shadowNear, GLsizei shadowFar, glm::vec3 position);

	///<summary>Set the viewport dimensions to that of the shadowMap and bind the shadowbuffer
	///<para>This should be called before the shadow pass.</para>
	///</summary>
	void setActive();

	///<summary>Get the texture that was created by the shadow pass.</summary>
	void getTexture();

	///<summary>Upload uniforms needed during the shadow pass.</summary>
	///<param name="shader">Shader for shadow pass that will use uniforms.</param>
	void uploadUniforms(const Shader& shader);

	///<summary>return the transform needed to be applied to the shadowmap based on its light's position.</summary>
	glm::mat4 getShadowTransform();

private:
	GLuint texture;
	GLuint shadowBuffer;
	GLsizei shadowWidth, shadowHeight, shadowFar, shadowNear;
	glm::vec3 position;
	DirectionLight& light;
};
