#pragma once
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader.h"
#include "glHelper.h"
#include "VertexData.h"
//#include <glm/gtc/type_ptr.hpp>

class ShadowMap {
public:
	///<summary>construct FBO for shadow map and set private member variables.
	///</summary>
	///<param name="position">The position of the orthographic projection matrix used to generate the shadow texture map. It is recommended to reference the poistion of the camera.</param>
	///<param name="direction">The direction associated with this shadow map. It is recommended to reference the direction the corresponding light is facing.</param>
	///<param name="resX">The x resolution of the shadow texture map.</param>
	///<param name="resY">The y resolution of the shadow texture map.</param>
	///<param name="shadowWidth">The width of the orthographic projection matrix used to generate the shadow texture map.</param>
	///<param name="shadowHeight">The height of the orthographic projection matrix used to generate the shadow texture map.</param>
	///<param name="shadowNear">The near bound of the orthographic projection matrix used to generate the shadow texture map.</param>
	///<param name="shadowFar">The far bound of the orthographic projection matrix used to generate the shadow texture map.</param>
	ShadowMap(const glm::vec3 position, const glm::vec3 direction, GLsizei resX, GLsizei resY, GLsizei shadowWidth, GLsizei shadowHeight, float shadowNear, float shadowFar);

	///<summary>Set the viewport dimensions to that of the shadowMap and bind the shadowbuffer
	///<para>This should be called before the shadow pass.</para>
	///</summary>
	void setActive();

	///<summary>Upload uniforms needed during the shadow pass.</summary>
	///<param name="shader">Shader for shadow pass that will use uniforms.</param>
	void uploadUniforms(const Shader& shader);

	///<summary>return the transform needed to be applied to the shadowmap based on its light's position.</summary>
	glm::mat4 getShadowTransform();

	///<summary>Get the texture that was created by the shadow pass.</summary>
	GLuint getTexture() { return this->texture; }

	void drawDebugCube(const Shader& shader);
	void drawDebugQuad(const Shader& shader);


private:
	GLuint texture;
	GLuint shadowBuffer;
	GLuint debugVBO, debugCubeVBO, debugVAO = 0, debugCubeVAO = 0;
	GLsizei shadowWidth, shadowHeight, shadowResX, shadowResY;
	GLfloat shadowFar, shadowNear;
	const glm::vec3 position, direction;
};
