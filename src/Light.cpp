#include "light.h"

Light::Light(
	const glm::vec3& ambient,
	const glm::vec3& diffuse,
	const glm::vec3& specular,
	const Shader& shader,
	const std::string& prefix
) :
	ambient(ambient),
	diffuse(diffuse),
	specular(specular),
	shader(shader),
	prefix(prefix)
{
}

const Shader Light::getShader() const
{
	return this->shader;
}

void Light::setShader(Shader& shader)
{
	this->shader = shader;
}

const glm::vec3 Light::getAmbient() const
{
	return this->ambient;
}

void Light::setAmbient(const glm::vec3& ambient)
{
	this->ambient = ambient;
}

const glm::vec3 Light::getDiffuse() const
{
	return this->diffuse;
}

void Light::setDiffuse(const glm::vec3& diffuse)
{
	this->diffuse = diffuse;
}

const glm::vec3 Light::getSpecular() const
{
	return this->specular;
}

void Light::setSpecular(const glm::vec3& specular)
{
	this->specular = specular;
}

void Light::addUpdateFunction(const std::string& name, std::function<void(Light*)> lf)
{
	this->updateFuncs[name] = lf;
}

void Light::runUpdateFuncs()
{
	for (std::map<std::string, std::function<void(Light*)>>::iterator it = this->updateFuncs.begin(); it != this->updateFuncs.end(); ++it)
	{
		it->second(this);
	}
}

void Light::uploadUniforms(const std::string& lightNum)
{
	this->uploadUniforms(lightNum);
}

void Light::uploadUniforms(Shader& shader, const std::string& lightNum) const
{
	shader.Use();
	shader.setVec3((this->prefix + "light[" + lightNum + "].ambient").c_str(), this->ambient);
	shader.setVec3((this->prefix + "light[" + lightNum + "].diffuse").c_str(), this->diffuse);
	shader.setVec3((this->prefix + "light[" + lightNum + "].specular").c_str(), this->specular);
}

GLuint Light::updateUniformBlock(GLuint ubo, GLuint start)
{
	GLuint size = start;
	glm::vec3 ambient = this->getAmbient();
	glm::vec3 diffuse = this->getDiffuse();
	glm::vec3 specular = this->getSpecular();
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(glm::vec4), glm::value_ptr(ambient));
	checkGLError("Light::updateUniformBlock - ambient");
	size += sizeof(glm::vec4);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(glm::vec4), glm::value_ptr(diffuse));
	checkGLError("Light::updateUniformBlock - diffuse");
	size += sizeof(glm::vec4);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(glm::vec4), glm::value_ptr(specular));
	checkGLError("Light::updateUniformBlock - specular");
	size += sizeof(glm::vec4);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	return size;
}


BasicLight::BasicLight(
	const glm::vec3& ambient,
	const glm::vec3& diffuse,
	const glm::vec3& specular,
	const glm::vec3& position,
	const Shader& shader,
	const std::string& prefix
) :
	Light(ambient, diffuse, specular, shader, prefix),
	position(position)
{
}

glm::vec3 BasicLight::getPosition() const
{
	return this->position;
}

void BasicLight::setPosition(glm::vec3 position)
{
	this->position = position;
}

void BasicLight::addUpdateFunction(const std::string& name, std::function<void(BasicLight*)> lf)
{
	this->updateFuncs[name] = lf;
}

void BasicLight::runUpdateFuncs()
{
	for (std::map<std::string, std::function<void(BasicLight*)>>::iterator it = this->updateFuncs.begin(); it != this->updateFuncs.end(); ++it)
	{
		it->second(this);
	}
}

void BasicLight::Draw()
{
	this->Draw(this->shader);
}

void BasicLight::Draw(Shader shader) const
{
	shader.Use();
	//Light::Draw(shader);

	glm::mat4 ModelMat = glm::mat4(1.0f);
	ModelMat = glm::scale(glm::translate(ModelMat, this->position), glm::vec3(0.25, 0.25, 0.25));
	shader.setMat4("M", ModelMat);

	shader.setVec3("light.ambient", this->ambient);
	shader.setVec3("light.diffuse", this->diffuse);
	shader.setVec3("light.specular", this->specular);
	glEnable(GL_DEPTH_TEST);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	checkGLError("BasicLight::Draw");
}

void BasicLight::uploadUniforms(Shader& shader, const std::string& lightNum) const
{
	Light::uploadUniforms(shader, lightNum);
	shader.setVec3((this->prefix + "light[" + lightNum + "].position").c_str(), this->position);
}

GLuint BasicLight::updateUniformBlock(GLuint ubo, GLuint start)
{
	GLuint size = Light::updateUniformBlock(ubo, start);
	glm::vec3 position = this->getPosition();
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(glm::vec4), glm::value_ptr(position));
	size += sizeof(glm::vec4);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	checkGLError("BasicLight::updateUniformBlock");
	return size;
}

void BasicLight::genVAO()
{
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, cubePositions.size() * sizeof(glm::vec3), &cubePositions.front(), GL_STATIC_DRAW);

	glBindVertexArray(this->VAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);

	this->definedVAO = true;

	glBindVertexArray(0);
	checkGLError("Light::setupLight");
}

const bool BasicLight::hasVAO() const
{
	return this->definedVAO;
}

void BasicLight::genShadowMap()
{
	glGenFramebuffers(1, &this->shadowFBO);
	checkGLError("BasicLight::genShadowMap");

	glGenTextures(1, &this->depthMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->depthMap);
	for (GLuint i = 0; i < 6; ++i) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, this->shadowWidth, this->shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	}
	checkGLError("BasicLight::genShadowMap");

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	checkGLError("BasicLight::genShadowMap");

	glBindFramebuffer(GL_FRAMEBUFFER, this->shadowFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, this->depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	checkGLError("BasicLight::genShadowMap");

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	else
		this->definedShadowMap = true;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	checkGLError("BasicLight::genShadowMap");

}

const bool BasicLight::hasShadowMap() const
{
	return this->definedShadowMap;
}

void BasicLight::drawShadowMap(std::function<void(Shader shader)> draw)
{
	glViewport(0, 0, this->shadowWidth, this->shadowHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, this->shadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	this->shadowCubeDepth.Use();
	std::vector<glm::mat4> transforms = this->getShadowTransforms();
	for (int i = 0; i < transforms.size(); i++) {
		this->shadowCubeDepth.setMat4("shadowTransform[" + std::to_string(i) + "]", transforms.at(i));
	}
	this->shadowCubeDepth.setFloat("far", 25.0f);
	this->shadowCubeDepth.setVec3("lightPos", this->position);
	checkGLError("BasicLight::drawShadowMap -- upload matrices");

	draw(this->shadowCubeDepth);
	checkGLError("BasicLight::drawShadowMap -- draw");
}

GLuint BasicLight::getDepthMap() const
{
	return this->depthMap;
}

std::vector<glm::mat4> BasicLight::getShadowTransforms()
{
	float aspect = (float)this->shadowWidth / (float)this->shadowHeight;
	float _near = 1.0f;
	float _far = 25.0f;
	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, _near, _far);

	std::vector<glm::mat4> shadowTransforms;
	shadowTransforms.push_back(shadowProj * glm::lookAt(this->position, this->position + glm::vec3( 1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(this->position, this->position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(this->position, this->position + glm::vec3( 0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(this->position, this->position + glm::vec3( 0.0,-1.0, 0.0), glm::vec3(0.0, 0.0,-1.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(this->position, this->position + glm::vec3( 0.0, 0.0, 1.0), glm::vec3(0.0,-1.0, 0.0)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(this->position, this->position + glm::vec3( 0.0, 0.0,-1.0), glm::vec3(0.0,-1.0, 0.0)));
	return shadowTransforms;
}

PointLight::PointLight(
	const glm::vec3& ambient,
	const glm::vec3& diffuse,
	const glm::vec3& specular,
	const glm::vec3& position,
	float constant,
	float linear,
	float quadratic,
	const Shader& shader,
	const std::string& prefix
) :
	BasicLight(ambient, diffuse, specular, position, shader, prefix),
	constant(constant), linear(linear), quadratic(quadratic)
{ 
}

const float PointLight::getConstant() const
{
	return this->constant;
}

void PointLight::setConstant(const float& constant)
{
	this->constant = constant;
}

const float PointLight::getLinear() const
{
	return this->linear;
}

void PointLight::setLinear(const float& linear)
{
	this->linear = linear;
}

const float PointLight::getQuadratic() const
{
	return this->quadratic;
}

void PointLight::setQuadratic(const float& quadratic)
{
	this->quadratic = quadratic;
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

void PointLight::uploadUniforms(Shader& shader, const std::string& lightNum) const
{
	BasicLight::uploadUniforms(shader, lightNum);
	shader.setFloat((this->prefix + "light[" + lightNum + "].constant").c_str(), this->constant);
	shader.setFloat((this->prefix + "light[" + lightNum + "].linear").c_str(), this->linear);
	shader.setFloat((this->prefix + "light[" + lightNum + "].quadratic").c_str(), this->quadratic);
}

GLuint PointLight::updateUniformBlock(GLuint ubo, GLuint start)
{
	GLuint size = BasicLight::updateUniformBlock(ubo, start);
	float constant = this->getConstant();
	float linear = this->getLinear();
	float quadratic = this->getQuadratic();
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(float), &constant);
	size += sizeof(float);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(float), &linear);
	size += sizeof(float);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(float), &quadratic);
	size += sizeof(float);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	size += 4;
	checkGLError("PointLight::updateUniformBlock");
	return size;
}

DirectionLight::DirectionLight(
	const glm::vec3& ambient,
	const glm::vec3& diffuse,
	const glm::vec3& specular,
	const glm::vec3& position,
	const glm::vec3& direction,
	const Shader& shader,
	const std::string& prefix
) :
	BasicLight(ambient, diffuse, specular, position, shader, prefix),
	direction(direction)
{
	this->genShadowMap();
}

glm::vec3 DirectionLight::getDirection() const
{
	return this->direction;
}

void DirectionLight::setDirection(glm::vec3 direction)
{
	this->direction = direction;
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

void DirectionLight::uploadUniforms(Shader& shader, const std::string& lightNum) const
{
	BasicLight::uploadUniforms(shader, lightNum);
	shader.setVec3((this->prefix + "light[" + lightNum + "].direction").c_str(), this->direction);
}

GLuint DirectionLight::updateUniformBlock(GLuint ubo, GLuint start)
{
	GLuint size = BasicLight::updateUniformBlock(ubo, start);
	glm::vec3 direction = this->getDirection();
	glm::mat4 lightSpaceMatrix = this->getShadowTransform();
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(glm::vec4), glm::value_ptr(direction));
	size += sizeof(glm::vec4);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(glm::mat4), glm::value_ptr(lightSpaceMatrix));
	size += sizeof(glm::mat4);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	checkGLError("DirectionLight::updateUniformBlock");
	return size;
}

void DirectionLight::genShadowMap()
{
	glGenFramebuffers(1, &this->shadowFBO);
	checkGLError("DirectionLight::genShadowMap");

	glGenTextures(1, &this->depthMap);
	glBindTexture(GL_TEXTURE_2D, this->depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	checkGLError("DirectionLight::genShadowMap");

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	checkGLError("DirectionLight::genShadowMap");

	glBindFramebuffer(GL_FRAMEBUFFER, this->shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	checkGLError("DirectionLight::genShadowMap");

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	else
		this->definedShadowMap = true;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	checkGLError("DirectionLight::genShadowMap");
}

void DirectionLight::drawShadowMap(std::function<void(Shader shader)> draw)
{
	glViewport(0, 0, 1024, 1024);
	glBindFramebuffer(GL_FRAMEBUFFER, this->shadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	this->shadowDepth.Use();
	this->shadowDepth.setMat4("lightSpaceMatrix", this->getShadowTransform());

	draw(this->shadowDepth);
}

glm::mat4 DirectionLight::getShadowTransform()
{
	glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 15.0f);
	glm::mat4 lightView = glm::lookAt(this->position, this->position + this->direction, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 lightSpaceMatrix = lightProjection * lightView;
	return lightSpaceMatrix;
}

SpotLight::SpotLight(
	const glm::vec3& ambient,
	const glm::vec3& diffuse,
	const glm::vec3& specular,
	const glm::vec3& position,
	const glm::vec3& direction,
	const float& constant,
	const float& linear,
	const float& quadratic,
	const float& cutOff,
	const float& outerCutOff,
	const Shader& shader,
	const std::string& prefix
) :
	PointLight(ambient, diffuse, specular, position, constant, linear, quadratic, shader, prefix),
	direction(direction), cutOff(cutOff), outerCutOff(outerCutOff)
{
}

glm::vec3 SpotLight::getDirection() const
{
	return this->direction;
}

void SpotLight::setDirection(glm::vec3 direction)
{
	this->direction = direction;
}

float SpotLight::getCutOff() const
{
	return this->cutOff;
}

void SpotLight::setCutOff(float cutOff)
{
	this->cutOff = cutOff;
}

float SpotLight::getOuterCutOff() const
{
	return this->outerCutOff;
}

void SpotLight::setOuterCutOff(float outerCutOff)
{
	this->outerCutOff = outerCutOff;
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

void SpotLight::uploadUniforms(Shader& shader, const std::string& lightNum) const
{
	PointLight::uploadUniforms(shader, lightNum);
	shader.setVec3((this->prefix + "light[" + lightNum + "].direction").c_str(), this->direction);
	shader.setFloat((this->prefix + "light[" + lightNum + "].cutOff").c_str(), this->cutOff);
	shader.setFloat((this->prefix + "light[" + lightNum + "].outerCutOff").c_str(), this->outerCutOff);
}

GLuint SpotLight::updateUniformBlock(GLuint ubo, GLuint start)
{
	GLuint size = PointLight::updateUniformBlock(ubo, start);
	glm::vec3 direction = this->getDirection();
	float cutOff = this->getCutOff();
	float outerCutOff = this->getOuterCutOff();
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(glm::vec4), glm::value_ptr(direction));
	size += sizeof(glm::vec4);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(float), &cutOff);
	size += sizeof(float);
	glBufferSubData(GL_UNIFORM_BUFFER, size, sizeof(float), &outerCutOff);
	size += sizeof(float);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	size += 8;
	checkGLError("SpotLight::updateUniformBlock");
	return size;
}

LightManager::LightManager() {}

void LightManager::addBasicLight(BasicLight& blight)
{
	this->basicLights.push_back(blight);
}

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

void LightManager::drawLights() {
	for (int i = 0; i < this->pointLights.size(); i++) {
		if (!this->pointLights[i].hasVAO())
			this->pointLights[i].genVAO();
	}
	for (int i = 0; i < this->basicLights.size(); i++) {
		if (!this->basicLights[i].hasVAO())
			this->basicLights[i].genVAO();
	}
	for (int i = 0; i < this->directionLights.size(); i++) {
		if (!this->directionLights[i].hasVAO())
			this->directionLights[i].genVAO();
	}
	for (int i = 0; i < this->spotLights.size(); i++) {
		//if (!this->spotLights[i].hasVAO())
		//	this->spotLights[i].genVAO();
	}

	for (int i = 0; i < this->pointLights.size(); i++) {
		this->pointLights[i].Draw();
	}
	for (int i = 0; i < this->basicLights.size(); i++) {
		this->basicLights[i].Draw();
	}
	for (int i = 0; i < this->directionLights.size(); i++) {
		this->directionLights[i].Draw();
	}
	for (int i = 0; i < this->spotLights.size(); i++) {
		//this->spotLights[i].Draw();
	}
}

void LightManager::uploadLightUniforms(Shader shader) {
	for (int i = 0; i < this->pointLights.size(); i++) {
		std::string lightNum = std::to_string(i);
		this->pointLights[i].uploadUniforms(shader, lightNum);
	}
	for (int i = 0; i < this->basicLights.size(); i++) {
		std::string lightNum = std::to_string(i);
		this->basicLights[i].uploadUniforms(shader, lightNum);
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
	//GLuint basicLightSize = sizeof(glm::vec3) * 4; // 48
	GLuint basicLightSize = 64;
	//GLuint pointLightSize = basicLightSize + sizeof(float) * 3; // 60
	GLuint pointLightSize = 80;
	//GLuint spotLightSize = pointLightSize + sizeof(glm::vec3) + 2 * sizeof(float); //80
	GLuint spotLightSize = 112;
	//GLuint directionLightSize = basicLightSize + sizeof(glm::vec3); //60
	GLuint directionLightSize = 80 + sizeof(glm::mat4);

	//pad sizes not divisible by 16

	//pointLightSize += 4;
	//directionLightSize += 4;

	GLuint uboSize = this->basicLights.size() * basicLightSize +
		this->pointLights.size() * pointLightSize +
		this->spotLights.size() * spotLightSize +
		this->directionLights.size() * directionLightSize;

	glGenBuffers(1, &this->ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, this->ubo);
	glBufferData(GL_UNIFORM_BUFFER, uboSize, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, 2, this->ubo, 0, uboSize);

	//set initial value
	checkGLError("LightManager::createUniformBlock");
	updateUniformBlock();
}

void LightManager::updateUniformBlock()
{
	GLuint size = 0;
	for (int i = 0; i < this->basicLights.size(); i++) {
		size = this->basicLights.at(i).updateUniformBlock(this->ubo, size);
	}
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
	for (int i = 0; i < this->basicLights.size(); i++) {
		this->basicLights.at(i).runUpdateFuncs();
	}
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

void LightManager::drawShadowMaps(std::function<void (Shader shader)> draw)
{
	//create depth mat if none exists
	for (int i = 0; i < this->basicLights.size(); i++) {
		if (!this->basicLights.at(i).hasShadowMap())
			this->basicLights.at(i).genShadowMap();
	}
	for (int i = 0; i < this->pointLights.size(); i++) {
		if (!this->pointLights.at(i).hasShadowMap())
			this->pointLights.at(i).genShadowMap();
	}
	for (int i = 0; i < this->directionLights.size(); i++) {
		if (!this->directionLights.at(i).hasShadowMap())
			this->directionLights.at(i).genShadowMap();
	}

	// draw depth map
	for (int i = 0; i < this->basicLights.size(); i++) {
		this->basicLights.at(i).drawShadowMap(draw);
	}
	for (int i = 0; i < this->pointLights.size(); i++) {
		this->pointLights.at(i).drawShadowMap(draw);
	}
	for (int i = 0; i < this->directionLights.size(); i++) {
		this->directionLights.at(i).drawShadowMap(draw);
	}
}
