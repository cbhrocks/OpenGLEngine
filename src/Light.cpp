#include "light.h"

Light::Light(
	glm::vec3 color,
	float ambient,
	float diffuse,
	float specular,
	const std::string prefix
) :
	color(color),
	ambient(ambient),
	diffuse(diffuse),
	specular(specular),
	prefix(prefix)
{
}

PointLight::PointLight(
	glm::vec3 position,
	glm::vec3 color,
	float ambient,
	float diffuse,
	float specular,
	float constant,
	float linear,
	float quadratic,
	const std::string prefix
) :
	Light(color, ambient, diffuse, specular, prefix),
	constant(constant), linear(linear), quadratic(quadratic), position(position)
{ 
	this->color = color;
}

void PointLight::addUpdateFunction(const std::string& name, std::function<void(PointLight*)> lf)
{
	this->updateFuncs[name] = lf;
}

void PointLight::runUpdateFuncs()
{
	for (std::map<std::string, std::function<void(PointLight*)>>::iterator it = this->updateFuncs.begin(); it != this->updateFuncs.end(); ++it)
	{
		it->second(this);
	}
}

void PointLight::uploadUniforms(const Shader& shader, const std::string& lightNum) const
{
	shader.setVec3((this->prefix + "light[" + lightNum + "].color"), this->color);
	shader.setFloat((this->prefix + "light[" + lightNum + "].ambient"), this->ambient);
	shader.setFloat((this->prefix + "light[" + lightNum + "].diffuse"), this->diffuse);
	shader.setFloat((this->prefix + "light[" + lightNum + "].specular"), this->specular);
	shader.setFloat((this->prefix + "light[" + lightNum + "].constant"), this->constant);
	shader.setFloat((this->prefix + "light[" + lightNum + "].linear"), this->linear);
	shader.setFloat((this->prefix + "light[" + lightNum + "].quadratic"), this->quadratic);
}

GLuint PointLight::updateUniformBlock(GLuint ubo, GLuint start)
{
	GLuint size = start;
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(float), &this->ambient);
	size += sizeof(float);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(float), &this->diffuse);
	size += sizeof(float);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(float), &this->specular);
	size += sizeof(float);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(float), &this->constant);
	size += sizeof(float);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(float), &this->linear);
	size += sizeof(float);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(float), &this->quadratic);
	size += sizeof(float) + 8;
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(glm::vec4), glm::value_ptr(this->color));
	size += sizeof(glm::vec4);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(glm::vec4), glm::value_ptr(this->position));
	size += sizeof(glm::vec4);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	checkGLError("PointLight::updateUniformBlock");
	return size;
}

DirectionLight::DirectionLight(
	glm::vec3 color,
	glm::vec3 direction,
	float ambient,
	float diffuse,
	float specular,
	const std::string prefix
) :
	Light(color, ambient, diffuse, specular, prefix),
	direction(direction)
{
}

void DirectionLight::addUpdateFunction(const std::string& name, std::function<void(DirectionLight*)> lf)
{
	this->updateFuncs[name] = lf;
}

void DirectionLight::runUpdateFuncs()
{
	for (std::map<std::string, std::function<void(DirectionLight*)>>::iterator it = this->updateFuncs.begin(); it != this->updateFuncs.end(); ++it)
	{
		it->second(this);
	}
}

void DirectionLight::uploadUniforms(const Shader& shader, const std::string& lightNum) const
{
	shader.setVec3((this->prefix + "light[" + lightNum + "].color"), this->color);
	shader.setFloat((this->prefix + "light[" + lightNum + "].ambient"), this->ambient);
	shader.setFloat((this->prefix + "light[" + lightNum + "].diffuse"), this->diffuse);
	shader.setFloat((this->prefix + "light[" + lightNum + "].specular"), this->specular);
	shader.setVec3((this->prefix + "light[" + lightNum + "].direction").c_str(), this->direction);
}

GLuint DirectionLight::updateUniformBlock(GLuint ubo, GLuint start)
{
	GLuint size = start;
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(float), &this->ambient);
	size += sizeof(float);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(float), &this->diffuse);
	size += sizeof(float);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(float), &this->specular);
	size += sizeof(float) + 4;
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(glm::vec4), glm::value_ptr(this->color));
	size += sizeof(glm::vec4);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(glm::vec4), glm::value_ptr(this->direction));
	size += sizeof(glm::vec4);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	checkGLError("DirectionLight::updateUniformBlock");
	return size;
}

SpotLight::SpotLight(
	glm::vec3 position,
	glm::vec3 direction,
	glm::vec3 color,
	float ambient,
	float diffuse,
	float specular,
	float constant,
	float linear,
	float quadratic,
	float cutOff,
	float outerCutOff,
	const std::string prefix
) :
	PointLight(position, color, ambient, diffuse, specular, constant, linear, quadratic, prefix),
	direction(direction), cutOff(cutOff), outerCutOff(outerCutOff)
{
}

void SpotLight::addUpdateFunction(const std::string& name, std::function<void(SpotLight*)> lf)
{
	this->updateFuncs[name] = lf;
}

void SpotLight::runUpdateFuncs()
{
	for (std::map<std::string, std::function<void(SpotLight*)>>::iterator it = this->updateFuncs.begin(); it != this->updateFuncs.end(); ++it)
	{
		it->second(this);
	}
}

void SpotLight::uploadUniforms(const Shader& shader, const std::string& lightNum) const
{
	PointLight::uploadUniforms(shader, lightNum);
	shader.setVec3((this->prefix + "light[" + lightNum + "].direction").c_str(), this->direction);
	shader.setFloat((this->prefix + "light[" + lightNum + "].cutOff").c_str(), this->cutOff);
	shader.setFloat((this->prefix + "light[" + lightNum + "].outerCutOff").c_str(), this->outerCutOff);
}

GLuint SpotLight::updateUniformBlock(GLuint ubo, GLuint start)
{
	GLuint size = start;
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(glm::vec4), glm::value_ptr(this->color));
	size += sizeof(glm::vec4);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(glm::vec4), glm::value_ptr(this->position));
	size += sizeof(glm::vec4);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(glm::vec4), glm::value_ptr(this->direction));
	size += sizeof(glm::vec4);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(float), &this->ambient);
	size += sizeof(float);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(float), &this->diffuse);
	size += sizeof(float);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(float), &this->specular);
	size += sizeof(float);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(float), &this->constant);
	size += sizeof(float);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(float), &this->linear);
	size += sizeof(float);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(float), &this->quadratic);
	size += sizeof(float);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(float), &this->cutOff);
	size += sizeof(float);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(float), &this->outerCutOff);
	size += sizeof(float);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	checkGLError("SpotLight::updateUniformBlock");
	return size;
}

LightManager::LightManager() {}

void LightManager::addPointLight(PointLight& plight)
{
	this->pointLights.push_back(plight);
}

void LightManager::addDirectionLight(DirectionLight& dlight)
{
	this->directionLights.push_back(dlight);
}

void LightManager::addSpotLight(SpotLight& slight)
{
	this->spotLights.push_back(slight);
}

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
		ModelMat = glm::scale(glm::translate(ModelMat, pointLights[i].getPosition()), glm::vec3(0.25, 0.25, 0.25));
		shader.setMat4("Model", ModelMat);

		shader.setVec3("light.color", pointLights[i].getColor());
		shader.setFloat("light.ambient", pointLights[i].getAmbient());
		shader.setFloat("light.diffuse", pointLights[i].getDiffuse());
		shader.setFloat("light.specular", pointLights[i].getSpecular());
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

void LightManager::uploadLightUniforms(Shader shader) {
	for (int i = 0; i < this->pointLights.size(); i++) {
		std::string lightNum = std::to_string(i);
		this->pointLights[i].uploadUniforms(shader, lightNum);
	}
	for (int i = 0; i < this->directionLights.size(); i++) {
		std::string lightNum = std::to_string(i);
		this->directionLights[i].uploadUniforms(shader, lightNum);
	}
	for (int i = 0; i < this->spotLights.size(); i++) {
		std::string lightNum = std::to_string(i);
		this->spotLights[i].uploadUniforms(shader, lightNum);
	}
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
		size = this->pointLights.at(i).updateUniformBlock(this->ubo, size);
	}
	for (int i = 0; i < this->spotLights.size(); i++) {
		size = this->spotLights.at(i).updateUniformBlock(this->ubo, size);
	}
	for (int i = 0; i < this->directionLights.size(); i++) {
		size = this->directionLights.at(i).updateUniformBlock(this->ubo, size);
	}
}

void LightManager::runUpdateFuncs()
{
	for (int i = 0; i < this->pointLights.size(); i++) {
		this->pointLights.at(i).runUpdateFuncs();
	}
	for (int i = 0; i < this->spotLights.size(); i++) {
		this->spotLights.at(i).runUpdateFuncs();
	}
	for (int i = 0; i < this->directionLights.size(); i++) {
		this->directionLights.at(i).runUpdateFuncs();
	}
}

void LightManager::drawShadowMaps(std::function<void (const Shader& shader)> draw)
{
	checkGLError("LightManager::drawShadowMaps -- generate shadow maps");
}
