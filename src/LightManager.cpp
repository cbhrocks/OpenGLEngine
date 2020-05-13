#include "LightManager.h"

LightManager::LightManager() {}

void LightManager::drawLights(const Shader& shader) {
	if (this->VAO == 0)
    {
		glGenVertexArrays(1, &this->VAO);
		glGenBuffers(1, &this->VBO);
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		glBufferData(GL_ARRAY_BUFFER, cubePositions.size() * sizeof(glm::vec3), &cubePositions.front(), GL_STATIC_DRAW);
		glBindVertexArray(this->VAO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
		glEnableVertexAttribArray(0);
		glBindVertexArray(0);

		//cleanup
		//glDeleteBuffers(1, &quadVBO);
    }
	for (int i = 0; i < this->pointLights.size(); i++) {
		shader.Use();
		//Light::Draw(shader);

		glm::mat4 ModelMat = glm::mat4(1.0f);
		ModelMat = glm::scale(glm::translate(ModelMat, pointLights.at(i)->getPosition()), glm::vec3(0.25, 0.25, 0.25));
		shader.setMat4("Model", ModelMat);

		shader.setVec3("light.color", pointLights.at(i)->getColor());
		shader.setFloat("light.ambient", pointLights.at(i)->getAmbient());
		shader.setFloat("light.diffuse", pointLights.at(i)->getDiffuse());
		shader.setFloat("light.specular", pointLights.at(i)->getSpecular());
		glEnable(GL_DEPTH_TEST);

		glBindVertexArray(this->VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		checkGLError("BasicLight::Draw");
	}
	//for (int i = 0; i < this->directionLights.size(); i++) {
	//	if (this->directionLights[i].hasVAO()) {
	//		this->directionLights[i].Draw(shader);
	//	}
	//}
	//for (int i = 0; i < this->spotLights.size(); i++) {
	//	//if (this->spotLights[i].hasVAO()){
	//		//this->spotLights[i].Draw(shader);
	//	//}
	//}
}

void LightManager::createUniformBlock()
{
	GLuint pointLightSize = 64;
	GLuint spotLightSize = 80;
	GLuint directionLightSize = 48;

	GLuint uboSize = this->pointLights.size() * pointLightSize +
		this->spotLights.size() * spotLightSize +
		this->directionLights.size() * directionLightSize;

	glGenBuffers(1, &this->ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, this->ubo);
	glBufferData(GL_UNIFORM_BUFFER, uboSize, NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, this->LIGHTS_UNIFORM_BLOCK_BINDING_POINT, this->ubo, 0, uboSize);

	//set initial value
	checkGLError("LightManager::createUniformBlock");
	updateUniformBlock();
}

void LightManager::updateUniformBlock()
{
	GLuint size = 0;
	for (int i = 0; i < this->pointLights.size(); i++) {
		size = this->pointLights.at(i)->updateUniformBlock(this->ubo, size);
	}
	for (int i = 0; i < this->spotLights.size(); i++) {
		size = this->spotLights.at(i)->updateUniformBlock(this->ubo, size);
	}
	for (int i = 0; i < this->directionLights.size(); i++) {
		size = this->directionLights.at(i)->updateUniformBlock(this->ubo, size);
	}
}
